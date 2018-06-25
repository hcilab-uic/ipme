#ifndef IPME_WB_SAGE_MESSAGES_H
#define IPME_WB_SAGE_MESSAGES_H

#include <string_view>
#include <vector>

namespace ipme {
namespace wb {
constexpr std::string_view add_client_msg =
    R"(
{
    "f":"0001",
    "d":
    {
        "clientType":"display",
        "requests":
        {
            "config":true,
            "version":true,
            "time":true,
            "console":false
        },
            "clientID":"0",
            "browser":"",
            "session":""
    }
}
)";

std::vector<std::string_view> subscribe_messages = {
    //    R"({"f":"0000","d":{"listener":"registerInteractionClient","alias":"0001"}})",
    R"({"f":"0000","d":{"listener":"getActiveClients","alias":"0002"}})",
    R"({"f":"0000","d":{"listener":"getRbac","alias":"0003"}})",
    R"({"f":"0000","d":{"listener":"updateAppState","alias":"0004"}})",
    R"({"f":"0000","d":{"listener":"updateStateOptions","alias":"0005"}})",
    R"({"f":"0000","d":{"listener":"appResize","alias":"0006"}})",
    R"({"f":"0000","d":{"listener":"appFullscreen","alias":"0007"}})",
    R"({"f":"0000","d":{"listener":"clearDisplay","alias":"0008"}})",
    //    R"({"f":"0000","d":{"listener":"updateSagePointerPosition","alias":"0000b"}})",
    R"({"f":"0000","d":{"listener":"createAppWindow","alias":"00009"}})",
    R"({"f":"0000","d":{"listener":"updateItemOrder","alias":"0000a}})"
    R"({"f":"0000","d":{"listener":"deleteAllApplications","alias":"000b"}})",
    R"({"f":"0000","d":{"listener":"tileApplications","alias":"000c"}})",
    R"({"f":"0000","d":{"listener":"addNewWebElement","alias":"000d"}})",
    R"({"f":"0000","d":{"listener":"openNewWebpage","alias":"000e"}})",
    R"({"f":"0000","d":{"listener":"startApplicationMove","alias":"000f"}})",
    R"({"f":"0000","d":{"listener":"startApplicationResize","alias":"0010"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationPosition","alias":"0011"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationPositionAndSize","alias":"0012"}})",
    R"({"f":"0000","d":{"listener":"finishApplicationMove","alias":"0013"}})",
    R"({"f":"0000","d":{"listener":"finishApplicationResize","alias":"0014"}})",
    R"({"f":"0000","d":{"listener":"deleteApplication","alias":"0015"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationState","alias":"0016"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationStateOptions","alias":"0017"}})",
    R"({"f":"0000","d":{"listener":"requestClientUpdate","alias":"0018"}})"};
} // namespace wb
} // namespace ipme

#endif // IPME_WB_SAGE_MESSAGES_H
