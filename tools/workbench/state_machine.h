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

    virtual inline bool is_uninitialized() const
    {
        return is_state(State::uninitialized);
    }

    virtual inline bool is_initialized() const
    {
        return is_state(State::initialized);
    }

    virtual inline bool is_running() const
    {
        return is_state(State::running);
    }

    virtual inline bool is_paused() const
    {
        return is_state(State::paused);
    }

    virtual inline bool is_stopped() const
    {
        return is_state(State::stopped);
    }

protected:
    virtual inline bool is_state(State state_value) const
    {
        return state() == state_value;
    }
};

} // namespace wb
} // namespace ipme

#endif // IPME_WB_STATE_MACHINE_H
