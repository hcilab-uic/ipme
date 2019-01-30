/* A tool for collecting, visualizing and annotating 3D Interaction data
 *
 * Copyright (C) 2017-2019 University of Illinois at Chicago
 *
 * Author: Harish G. Naik <hnaik2@uic.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
