#include <filesystem>
#include <fstream>
#include <iostream>

#include "google/protobuf/util/json_util.h"
#include "protobuf/scene.pb.h"

int main(int argc, char** argv)
{
    std::filesystem::path input_file{argv[1]};
    std::filesystem::path output_file{argv[2]};

    std::ifstream in{input_file};

    ipme::scene::Scene scene;

    scene.ParseFromIstream(&in);

    std::string json_message;
    google::protobuf::util::JsonPrintOptions print_options;
    print_options.add_whitespace = true;
    print_options.always_print_primitive_fields = true;
    print_options.preserve_proto_field_names = true;

    google::protobuf::util::MessageToJsonString(scene, &json_message,
                                                print_options);
    std::ofstream out{output_file};

    out.write(json_message.c_str(), json_message.size());

    return EXIT_SUCCESS;
}
