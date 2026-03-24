# stock-financials

A multi-component C++ application for fetching, storing, and browsing public company financial data sourced from the SEC (U.S. Securities and Exchange Commission) EDGAR system. The project is split into four modules: a **database library**, a **data processor**, a **REST API**, and a **terminal UI (TUI)**.

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Prerequisites](#prerequisites)
- [Project Structure](#project-structure)
- [Modules](#modules)
  - [database](#database)
  - [data-processor](#data-processor)
  - [api](#api)
  - [tui](#tui)
- [Building](#building)
- [Configuration](#configuration)
- [Running the Application](#running-the-application)
- [API Reference](#api-reference)
- [Data Models](#data-models)
- [Financial Facts Reference](#financial-facts-reference)
- [Derived Financial Metrics](#derived-financial-metrics)

---

## Architecture Overview

```
┌─────────────────────────────────────────────────┐
│                   TUI (ncurses)                  │
│         Terminal interface for browsing          │
└─────────────────────┬───────────────────────────┘
                      │ HTTP (libcurl)
┌─────────────────────▼───────────────────────────┐
│               REST API (cpp-httplib)             │
│         Serves company & financial data          │
└─────────────────────┬───────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────┐
│            Database (SQLite via sqlite_modern)   │
│               libstockdatabase.a                 │
└─────────────────────┬───────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────┐
│           Data Processor (libcurl + OpenSSL)     │
│   Fetches SEC EDGAR bulk data & stock prices     │
└─────────────────────────────────────────────────┘
```

The **data processor** is a one-time (or periodic) job that populates the SQLite database. The **API** reads from that database and serves JSON over HTTP. The **TUI** queries the API to render an interactive terminal dashboard.

---

## Prerequisites

The following libraries must be installed on your system:

| Library | Used By | Purpose |
|---|---|---|
| `libsqlite3` | database, api, data-processor | SQLite database engine |
| `libcurl` | data-processor, tui | HTTP requests |
| `libcrypto` (OpenSSL) | data-processor | SHA-256 file hashing |
| `libncurses` | tui | Terminal UI rendering |
| `nlohmann/json` (header-only) | all | JSON parsing and serialization |
| `sqlite_modern_cpp` (header-only) | database | C++ SQLite wrapper |
| `cpp-httplib` (header-only) | api | HTTP server |
| `rapidfuzz-cpp` (header-only) | tui | Fuzzy company name search |

A C++23-compatible compiler (e.g., GCC 13+ or Clang 16+) and `make` are required.

---

## Project Structure

```
stock-financials/
├── database/               # Shared static library (libstockdatabase.a)
│   ├── include/
│   │   ├── database.h
│   │   ├── models/         # Company, Filing, FinancialFact, StockPrice, etc.
│   │   └── repositories/   # Data access layer
│   └── src/
│       ├── database.cpp
│       └── repositories/
├── data-processor/         # Standalone binary: populates the database
│   ├── include/
│   └── src/
│       ├── application.cpp # Orchestrates the full data pipeline
│       ├── curl_wrapper.cpp# Wraps all outbound HTTP calls
│       ├── data_parser.cpp # Parses SEC EDGAR JSON company facts
│       └── utils.cpp
├── api/                    # Standalone binary: HTTP REST API
│   ├── include/
│   └── src/
│       ├── main.cpp
│       ├── server.cpp
│       ├── controllers/
│       └── services/
├── tui/                    # Standalone binary: terminal user interface
│   ├── include/
│   └── src/
│       ├── api_client.cpp
│       └── ui/
└── financial_mapping.json  # Canonical name → SEC XBRL tag mapping
```

---

## Modules

### database

A static library (`libstockdatabase.a`) shared by the data processor and the API. It wraps SQLite via `sqlite_modern_cpp` and exposes a repository pattern for all data access.

**Repositories:**

| Repository | Responsibility |
|---|---|
| `CompanyRepository` | Upsert and query companies by CIK |
| `FilingRepository` | Upsert and query SEC filings (10-K, 10-Q, etc.) by CIK or accession number |
| `FinancialFactRepository` | Upsert and query individual financial facts tied to a filing |
| `StockPriceRepository` | Upsert and query daily stock price records tied to a filing date |
| `CompanyFactsMetadataRepository` | Track SHA-256 hashes of processed company fact files to skip unchanged data |

Build the library before building any other module.

---

### data-processor

A one-shot CLI binary that downloads raw financial data from public sources and populates the SQLite database.

**Pipeline (`Application::run`):**

1. **Fetch company tickers** — Downloads `company_tickers.json` from the SEC and upserts every company (CIK, ticker, title) into the database.
2. **Download bulk EDGAR data** — Downloads the full `companyfacts.zip` archive from SEC EDGAR (~several GB) to a temporary directory and unzips it.
3. **Process company facts** — For each company JSON file in the archive:
   - Checks the SHA-256 hash against the stored hash. Skips the file if it hasn't changed.
   - Parses filings and financial facts using `DataParser`.
   - Fetches daily stock price history from the Twelve Data API and links each price record to its closest filing date.
   - Stores the new hash on success.
4. **Clean up** — Deletes the temporary directory.

**External APIs used:**

| API | Data |
|---|---|
| `https://www.sec.gov/files/company_tickers.json` | All registered public companies |
| `https://www.sec.gov/Archives/edgar/daily-index/xbrl/companyfacts.zip` | Bulk EDGAR financial facts archive |
| `https://api.twelvedata.com/time_series` | Historical daily stock prices (requires API key) |

The Twelve Data API key is read from a `config.json` file in the working directory:

```json
{
  "key": "YOUR_TWELVE_DATA_API_KEY"
}
```

---

### api

An HTTP REST API server built with `cpp-httplib`. It reads from the SQLite database and serves JSON responses on port `8080`.

**Startup:**

The database path is resolved in this order:
1. `STOCK_DB_PATH` environment variable
2. Fallback: `../data/stock_data.db` (relative to the binary)

The server listens on `0.0.0.0:8080`.

See [API Reference](#api-reference) for endpoint details.

---

### tui

An interactive terminal application built with `ncurses`. It connects to the running API server over HTTP using `libcurl`.

**Pages:**

| Page | Description |
|---|---|
| Search | Fuzzy-search companies by name using `rapidfuzz`. Displays matching results as a selectable list. |
| Company | Displays financial reports for the selected company — facts, filing metadata, and stock price at filing date. |
| Filings | Lists all SEC filings for the selected company. |

> **Note:** The API base URL is currently hardcoded to `http://10.0.0.210:8080` in `tui/src/api_client.cpp`. Update this to match your API server's address before building.

---

## Building

Each module is built with its own `Makefile`. Build the `database` library first, as the other modules depend on it.

```bash
# 1. Build the shared database library
cd database
make

# 2. Build the data processor
cd ../data-processor
make

# 3. Build the API
cd ../api
make

# 4. Build the TUI
cd ../tui
make
```

Binaries are placed in each module's `bin/` directory:

| Module | Binary |
|---|---|
| data-processor | `data-processor/bin/data_proc` |
| api | `api/bin/api` |
| tui | `tui/bin/tui` |

To clean all build artifacts in a module, run `make clean` inside that module's directory.

---

## Configuration

### API key (data-processor)

Create `data-processor/config.json`:

```json
{
  "key": "YOUR_TWELVE_DATA_API_KEY"
}
```

### Database path (API)

Set the `STOCK_DB_PATH` environment variable to point to your SQLite database file, or place the database at `../data/stock_data.db` relative to the `api/bin/` directory.

### API server address (TUI)

Edit `tui/src/api_client.cpp` and replace `http://10.0.0.210:8080` with the address of your running API server.

---

## Running the Application

### Step 1 — Populate the database

Run the data processor once to download and import all financial data. This may take a significant amount of time due to the size of the SEC bulk archive.

```bash
cd data-processor
./bin/data_proc
```

### Step 2 — Start the API server

```bash
cd api
STOCK_DB_PATH=/path/to/stock_data.db ./bin/api
# Output: Api currently running on port: 8080
```

### Step 3 — Launch the TUI

```bash
cd tui
./bin/tui
```

---

## API Reference

All responses are `application/json`. All endpoints are read-only (`GET`).

---

### `GET /companies`

Returns a list of all companies in the database.

**Response**

```json
[
  { "cik": "320193", "ticker": "AAPL", "title": "Apple Inc." },
  ...
]
```

---

### `GET /companies/:cik`

Returns a single company by its CIK number.

**Path Parameters**

| Parameter | Type | Description |
|---|---|---|
| `cik` | integer | SEC Central Index Key |

**Response** — `200 OK`

```json
{ "cik": "320193", "ticker": "AAPL", "title": "Apple Inc." }
```

**Error** — `404 Not Found`

```json
{ "error": "Company does not exist" }
```

---

### `GET /companies/:cik/financials`

Returns financial reports for a company, grouped by filing.

**Path Parameters**

| Parameter | Type | Description |
|---|---|---|
| `cik` | integer | SEC Central Index Key |

**Query Parameters**

| Parameter | Type | Default | Description |
|---|---|---|---|
| `period` | string | `annual` | Reporting period: `annual` or `quarterly` |
| `limit` | integer | *(none)* | Maximum number of filings to return (most recent first) |
| `facts` | string | *(all)* | Comma-separated list of fact names to include (e.g., `Revenue,Net Income`) |

**Example**

```
GET /companies/320193/financials?period=annual&limit=5&facts=Revenue,Net%20Income
```

**Response** — `200 OK`

```json
{
  "company": { "cik": "320193", "ticker": "AAPL", "title": "Apple Inc." },
  "reports": [
    {
      "filing": {
        "filingId": 1,
        "accession": "0000320193-23-000106",
        "cik": "320193",
        "form": "10-K",
        "fy": 2023,
        "fp": "FY",
        "filedDate": "2023-11-03"
      },
      "facts": [
        {
          "tag": "Revenue",
          "startDate": "2022-09-25",
          "endDate": "2023-09-30",
          "value": 383285000000,
          "unit": "USD",
          "sourceTag": "RevenueFromContractWithCustomerExcludingAssessedTax"
        }
      ],
      "stockPrice": {
        "date": "2023-11-03",
        "currency": "USD",
        "open": 176.38,
        "high": 176.38,
        "low": 173.93,
        "close": 174.91,
        "volume": 53109340
      }
    }
  ]
}
```

**Errors**

| Status | Condition |
|---|---|
| `404` | Company not found, or invalid `period` / `limit` parameters |

---

### `GET /companies/:cik/filings`

Returns all SEC filings for a company.

**Path Parameters**

| Parameter | Type | Description |
|---|---|---|
| `cik` | integer | SEC Central Index Key |

**Response** — `200 OK`

```json
[
  {
    "filingId": 1,
    "accession": "0000320193-23-000106",
    "cik": "320193",
    "form": "10-K",
    "fy": 2023,
    "fp": "FY",
    "filedDate": "2023-11-03"
  },
  ...
]
```

---

## Data Models

### Company

| Field | Type | Description |
|---|---|---|
| `cik` | string | SEC Central Index Key (unique identifier) |
| `ticker` | string | Stock ticker symbol |
| `title` | string | Full legal company name |

### Filing

| Field | Type | Description |
|---|---|---|
| `filingId` | integer | Internal database ID |
| `accession` | string | SEC accession number (e.g., `0000320193-23-000106`) |
| `cik` | string | Company CIK |
| `form` | string | Form type (e.g., `10-K`, `10-Q`, `10-K/A`) |
| `fy` | integer | Fiscal year |
| `fp` | string | Fiscal period (e.g., `FY`, `Q1`, `Q2`, `Q3`) |
| `filedDate` | string | Date the filing was submitted (YYYY-MM-DD) |

### FinancialFact

| Field | Type | Description |
|---|---|---|
| `tag` | string | Canonical fact name (e.g., `Revenue`) |
| `startDate` | string | Period start date (YYYY-MM-DD) |
| `endDate` | string | Period end date (YYYY-MM-DD) |
| `value` | number | Reported value |
| `unit` | string | Unit of measurement (e.g., `USD`, `shares`) |
| `sourceTag` | string | Original SEC XBRL tag (e.g., `RevenueFromContractWithCustomerExcludingAssessedTax`) |

### StockPrice

| Field | Type | Description |
|---|---|---|
| `date` | string | Trading date (YYYY-MM-DD) |
| `currency` | string | Currency code (e.g., `USD`) |
| `open` | number | Opening price |
| `high` | number | Daily high |
| `low` | number | Daily low |
| `close` | number | Closing price |
| `volume` | number | Shares traded |

---

## Financial Facts Reference

The `financial_mapping.json` file maps human-readable canonical names to their underlying SEC XBRL tags. These canonical names are the values used in the `facts` query parameter.

| Canonical Name | Description |
|---|---|
| `Revenue` | Total revenue from operations |
| `Gross Profit` | Revenue minus cost of goods sold |
| `Operating Income` | Income from operations before interest and tax |
| `Net Income` | Bottom-line profit after all expenses and taxes |
| `Earnings Per Share Basic` | Net income per basic share |
| `Earnings Per Share Diluted` | Net income per diluted share |
| `Cash And Cash Equivalents` | Cash and liquid assets on hand |
| `Accounts Receivable` | Amounts owed by customers |
| `Cash Flow` | Net cash from continuing operating activities |
| `Overall Debt` | Total long-term debt |
| `Current Debt` | Current portion of long-term debt |
| `Long Term Debt` | Non-current long-term debt |
| `Commercial Paper` | Short-term unsecured debt instruments |
| `Stockholders' Equity` | Net assets attributable to shareholders |
| `Shares` | Common shares outstanding |
| `Total Assets` | Sum of all assets |
| `Current Assets` | Assets expected to convert to cash within one year |
| `Inventory` | Net inventory value |
| `Total Liabilities` | Sum of all liabilities |
| `Current Liabilities` | Liabilities due within one year |
| `Capital Expenditures` | Payments for property, plant and equipment |
| `Goodwill` | Goodwill from acquisitions |
| `Intangible Assets` | Intangible assets excluding goodwill |
| `Dividends Per Share` | Dividends declared per common share |

---

## Derived Financial Metrics

The API automatically derives the following metrics and appends them to each report's `facts` array when the underlying data is present. These do not come directly from SEC filings.

| Derived Fact | Formula | Notes |
|---|---|---|
| **P/E Ratio** | `Stock Price / Earnings Per Share (Diluted)` | Requires stock price data for the filing date |
| **Tangible Price-to-Book** | `Stock Price / (Tangible Book Value per Share)` | Tangible Book = Total Assets − Total Liabilities − Goodwill − Intangible Assets |
| **Current Ratio** | `Current Assets / Current Liabilities` | Measure of short-term liquidity |
| **Debt-to-Equity Ratio** | `Total Liabilities / Long-Term Debt` | Measure of financial leverage |

Amendment filings (`10-K/A`, `10-Q/A`) are automatically detected and used to update the facts of their original counterpart filing rather than appearing as separate entries.
