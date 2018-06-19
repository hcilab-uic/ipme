#ifndef IPME_WB_STATE_MACHINE_H
#define IPME_WB_STATE_MACHINE_H

namespace ipme {
namespace wb {
class State_machine {
public:
    enum class State
    {
        uninitialized,
        initialized,
        running,
        paused,
        stopped
    };

    virtual State state() const = 0;
    virtual void set_state(const State state) = 0;
};

} // namespace wb
} // namespace ipme

#endif // IPME_WB_STATE_MACHINE_H
