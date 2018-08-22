import copy
import logging
import subprocess
import sys

import numpy as np

from pathlib import Path

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

try:
    from pb import scene_pb2
except ModuleNotFoundError:
    print('module pb.scene_pb2 does not exist, compiling')
    script_dir = Path(sys.argv[0]).parent.absolute()
    proto_src_dir = Path(script_dir).parent / 'lib/protobuf'
    proto_src_path = proto_src_dir / 'scene.proto'
    module_dest_path = script_dir / 'pb'
    module_dest_path.mkdir(parents=True, exist_ok=True)
    cmd = ['/usr/bin/protoc', '-I', proto_src_dir, '--python_out',
           module_dest_path, proto_src_path]
    subprocess.run(cmd)
    from pb import scene_pb2


class Experiment(object):
    def __init__(self, input_path):
        self.__input_path = Path(input_path)
        self.__file_paths = []
        self.__screen_offset = None
        self.__registered_objects = {}
        self.__scenes = {}
        self.__config = None
        self.__load_files()
        self.__computed_frames = []
        self.__compute_filtered_frames()

    @property
    def config(self):
        return self.__config

    @property
    def scenes(self):
        return self.__scenes

    @property
    def filtered_frames(self):
        frames = []
        for _, scene in self.__scenes.items():
            frames.extend(Experiment.filter_frames(scene))
        return frames

    @property
    def table_from_filtered_frames(self):
        return Experiment.table_from_frames(self.filtered_frames)

    @property
    def computed_frames(self):
        return self.__computed_frames

    def __load_files(self):
        if self.__input_path.is_dir():
            for protobuf_path in sorted(self.__input_path.glob('**/*.pb')):
                self.__load_file(protobuf_path.absolute())
        else:
            self.__load_file(self.__input_path.absolute())

    def __load_file(self, file_path):
        logger.debug('loading file {}'.format(file_path))
        scene = scene_pb2.Scene()
        with open(str(file_path), 'rb') as f:
            scene.ParseFromString(f.read())
        stub = file_path.parent.name
        self.__scenes.update({stub: scene})

        if not self.__config:
            self.__config = scene.config
            offset = self.__config.screen_offset
            self.__screen_offset = np.array([offset.x, offset.y, offset.z])
            self.__screen_offset

            for registered_object in self.__config.registered_objects:
                self.__registered_objects.update({
                    registered_object.id: {
                        'name': registered_object.name,
                        'type': registered_object.type
                    }
                }
            )

    @staticmethod
    def create_object(pose):
        rotation = pose.orientation
        return {
            'position': np.array([
                pose.position.x,
                pose.position.y,
                pose.position.z
            ]),
            'rotation': np.quaternion(
                rotation.w,
                rotation.x,
                rotation.y,
                rotation.z
            )
        }

    def __compute_filtered_frames(self):
        for frame in self.filtered_frames:
            frame_dict = {'persons': [], 'devices': []}
            for obj in frame.vrpn_objects:
                object_structure = Experiment.create_object(obj.pose)
                try:
                    registered_object = self.__registered_objects[
                        obj.vrpn_source_id]
                    name = registered_object['name']
                    if 'person' in name:
                        frame_dict['persons'].append(object_structure)
                    elif 'computer' in name:
                        frame_dict['devices'].append(object_structure)
                    else:
                        raise Exception('error: {}'.format(name))
                except KeyError as err:
                    logger.error('unknown vrpn_source_id {}'.format(err))

            for obj in frame.screen_objects:
                posn = obj.position
                adjusted_pos = np.array([
                    posn.x + self.__screen_offset[0],
                    posn.y + self.__screen_offset[1],
                    posn.z + self.__screen_offset[2]
                ])
                quaternion = np.quaternion(0, 0, 0, 1)
                frame_dict['devices'].append({
                    'position': adjusted_pos,
                    'rotation': quaternion
                })

            self.__computed_frames.append(frame_dict)

    @staticmethod
    def filter_frames(scene):
        frames_with_screen_objects = []
        for frame in scene.frames:
            if len(frame.screen_objects) > 0:
                frames_with_screen_objects.append(frame)
        return frames_with_screen_objects

    @staticmethod
    def table_from_frames(frames):
        table = []
        for frame in frames:
            row = [frame.frame_id, frame.timestamp]

            for obj in frame.vrpn_objects:
                row.extend([
                    obj.vrpn_source_id,
                    obj.timestamp,
                    obj.device_tag,
                    obj.service_type,
                    obj.type,
                    obj.flags
                ])
                row.extend(Experiment.list_from_pose(obj.pose))

            for obj in frame.screen_objects:
                row.extend([obj.screen_object_id] +
                           Experiment.list_from_position(obj.position) +
                           Experiment.list_from_size(obj.size))

            for obj in frame.simple_objects:
                row.extend([obj.object_id, obj.timestamp] +
                           Experiment.list_from_pose(obj.pose))
            table.append(row)
        return table

    @staticmethod
    def list_from_pose(pose):
        return Experiment.list_from_position(pose.position) + \
               Experiment.list_from_orientation(pose.orientation)

    @staticmethod
    def list_from_position(position):
        return [position.x, position.y, position.z]

    @staticmethod
    def list_from_orientation(orientation):
        return [orientation.w, orientation.x, orientation.y, orientation.z]

    @staticmethod
    def list_from_size(size):
        return [size.width, size.height]


def load_experiment(input_path):
    return Experiment(input_path)


def load_scenes(input_path):
    return load_experiment(input_path).scenes
