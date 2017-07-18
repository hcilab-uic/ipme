#ifndef IPME_IO_VICONCSVWRITER_H
#define IPME_IO_VICONCSVWRITER_H

#include <fstream>

#include "data/vicon_point.h"
#include "vicon_datahandler.h"

namespace ipme {
namespace sensor {
namespace oc = omicronConnector;

class Vicon_csvwriter : public Vicon_datahandler {
  public:
    Vicon_csvwriter(const std::string& outfilename, const bool kinect_only);

    /**
     * virtual destructor
     */
    virtual ~Vicon_csvwriter();

    /*! @copydoc Vicon_datahandler::handle_trace(const oc::EventData&)
     */
    virtual void handle_trace(const oc::EventData& event) override;

    /*! @copydoc Vicon_datahandler::handle_untrace(const oc::EventData&)
     */
    virtual void handle_untrace(const oc::EventData& event) override;

    /*! @copydoc Vicon_datahandler::handle_update(const oc::EventData&)
     */
    virtual void handle_update(const oc::EventData& event) override;

    /*! @copydoc Vicon_datahandler::handle_up(const oc::EventData&)
     */
    virtual void handle_up(const oc::EventData& event) override;

    /*! @copydoc Vicon_datahandler::handle_down(const oc::EventData&)
     */
    virtual void handle_down(const oc::EventData& event) override;

  private:
    std::ofstream ofs_;
    const bool kinect_only_;

    unsigned int frame_id_ = 0;
};
} // namespace sensor
} // namespace ipme

#endif //  IPME_IO_VICONCSVWRITER_H
