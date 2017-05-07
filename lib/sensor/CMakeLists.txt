set(SRCS
    sensor.cpp
    walabot.cpp
    # Adding these headers so that QtCreator IDE can show them in projects view
    # I know this is dumb, but there is no other way to do it
    receiver.h
    receiver3d.h receiver3d.ipp
    sensor.h
    walabot.h
    walabot3d.h walabot3d.ipp
    walabot_receiver_writer.h walabot_receiver_writer.ipp
    )

add_library(sensor ${SRCS})
