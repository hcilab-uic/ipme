#ifndef IPME_SENSOR_VICON_3DWRITER_H
#define IPME_SENSOR_VICON_3DWRITER_H

#include <boost/filesystem.hpp>

#include "data/point3d.h"
#include "vicon_datahandler.h"
#include "visualization/polydata.h"

namespace ipme {
namespace sensor {

class Vicon_3dwriter : public Vicon_datahandler {
  public:
    Vicon_3dwriter(const boost::filesystem::path& outfile_name)
        : outfile_name_{outfile_name}
    {
    }

    /**
     * virtual destructor
     */
    virtual ~Vicon_3dwriter();

    /**
     * Write output to file
     * @param[in] outfile_name
     */
    void write(const boost::filesystem::path& outfile_name);

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

  public:
    boost::filesystem::path outfile_name_;
    visualization::Polydata<data::Point3d<double>> polydata_writer_;
};

} // namespace sensor
} // namespace ipme

#endif // IPME_SENSOR_VICON_3DWRITER_H
