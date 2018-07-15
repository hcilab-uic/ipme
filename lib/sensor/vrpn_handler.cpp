#include "vrpn_handler.h"

#include "utils/logger.h"

namespace ipme {
namespace sensor {
Vrpn_handler::Vrpn_handler(std::shared_ptr<data::Scene> scene) : scene_{scene}
{
}

void Vrpn_handler::handle_trace(const omicronConnector::EventData& /*event*/)
{
}

void Vrpn_handler::handle_untrace(const omicronConnector::EventData& /*event*/)
{
}

void Vrpn_handler::handle_update(const omicronConnector::EventData& event)
{
    scene_->add_object(event);
}

void Vrpn_handler::handle_up(const omicronConnector::EventData& /*event*/)
{
}

void Vrpn_handler::handle_down(const omicronConnector::EventData& /*event*/)
{
}

} // namespace sensor
} // namespace ipme
