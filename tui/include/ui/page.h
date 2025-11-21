#pragma once

class Page {
  public:
    bool refreshNeeded;

    virtual ~Page() = default;

    virtual void render() = 0;
    virtual void handleInput(int /*ch*/) {}
    virtual bool needsRefresh() const { return refreshNeeded; }
};
