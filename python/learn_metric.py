#!/usr/bin/env python3

import numpy as np
import sys

from argparse import ArgumentParser
from pathlib import Path

# IPME project specific
try:
    from pb import scene_pb2
except:
    print('no protobuf support module found.please run ./setup.sh in the '
          'script directory')
    exit(1)


def parse_args():
    parser = ArgumentParser(sys.argv)
    parser.add_argument('--input', required=True)
    parser.add_argument('--output')
    parser.add_argument('--min-screen-objects', type=int, default=1)
    parser.add_argument(
        '--vrpn-filter-policy', choices=['avg', 'first', 'last'], default='avg',
        help='filters VRPN objects based on the stated filtering policy\n' \
             'avg: aggregate all positions and rotations and average\n'
             'first: pick the first for that object_id\n'
             'middle: pick the middle for that object_id\n'
             'last: pick the last for that id'
    )
    parser.add_argument('--registered-object-type-identifier',
                        choices=['text', 'enum'], default='text')

    return parser.parse_args()


def read_scene(file_path):
    scene = scene_pb2.Scene()

    with open(file_path, 'rb') as f:
        scene.ParseFromString(f.read())

    return scene


class SceneProcessor(object):
    def __init__(self, options):
        self.__o = options
        self.__scene = scene_pb2.Scene()
        self.__read()
        self.__process_registered_object_map()

    def process(self):
        for frame in self.__scene.frames:
            if not self.__apply_filter(frame):
                continue
            filtered_vrpn_objects = self.__apply_vrpn_filter_policy(
                frame.vrpn_objects)
            affinities = self.__compute_all_pair_affinities(
                filtered_vrpn_objects, frame.screen_objects
            )
            print(affinities)
            # print('frame_id={frame_id}, timestamp={timestamp}, '
            #       'vrpn_object_count={vrpn_object_count}, '
            #       'screen_object_count={screen_object_count}'.format(
            #            frame_id=frame.frame_id, timestamp=frame.timestamp,
            #            vrpn_object_count=len(frame.vrpn_objects),
            #            screen_object_count=len(frame.screen_objects)))

    def __compute_all_pair_affinities(self, vrpn_objects, screen_objects):
        users = self.__extract_object_of_type(
            scene_pb2.Registered_object.HUMAN, vrpn_objects)
        devices = self.__extract_object_of_type(
            scene_pb2.Registered_object.DEVICE, vrpn_objects)

        try:
            active_object_count = len(users)
            passive_object_count = len(devices) + len(screen_objects)
            print('active count={a}, passive count={p}'.format(
                a=active_object_count, p=passive_object_count))
            affinities = np.zeros([active_object_count, passive_object_count])
            return affinities
        except TypeError:
            print('Error in frame')
            print(users)
            print(devices)
            print(screen_objects)

    def __extract_object_of_type(self, object_type, objects):
        filtered_objects = []
        for o in objects:
            try:
                if self.__ro_map[o.vrpn_source_id] == object_type:
                    filtered_objects.append(o)
            except KeyError as err:
                pass
                # print(err)
        return filtered_objects

    def __apply_vrpn_filter_policy(self, vrpn_objects):
        policy_name = self.__o.vrpn_filter_policy
        if policy_name == 'avg':
            return SceneProcessor.vrpn_avg_policy(vrpn_objects)

        raise Exception('{} policy not implemented'.format(policy_name))

    @staticmethod
    def vrpn_avg_policy(vrpn_objects):
        objects = {}
        for o in vrpn_objects:
            if o.vrpn_source_id not in objects:
                objects.update({o.vrpn_source_id: []})

            objects[o.vrpn_source_id].append(SceneProcessor.pose_to_array(
                o.pose))

        average_rows = []
        for _, value in objects.items():
            shape = [len(value), value[0].shape[0]]
            m = np.concatenate(value).reshape(shape)
            average_rows.append(np.mean(m, axis=0))

        shape = [len(objects), average_rows[0].shape[0]]
        return np.concatenate(average_rows).reshape(shape)

    @staticmethod
    def pose_to_array(pose):
        return np.array([
            pose.position.x, pose.position.y, pose.position.z,
            pose.orientation.w, pose.orientation.x, pose.orientation.y,
            pose.orientation.z
        ])

    def __apply_filter(self, frame):
        process = True
        if len(frame.screen_objects) < self.__o.min_screen_objects:
            process = False

        return process

    def __process_registered_object_map(self):
        self.__ro_map = {}
        human = scene_pb2.Registered_object.HUMAN
        device = scene_pb2.Registered_object.DEVICE
        for ro in self.__scene.config.registered_objects:
            ro_type = ro.type
            if self.__o.registered_object_type_identifier == 'text':
                ro_type = human if 'person' in ro.name else device
            self.__ro_map.update({ro.id: ro_type})

    def __repr__(self):
        config = self.__scene.config
        offset = config.screen_offset
        s = 'screen offset: ({x},{y},{z})\n'.format(
            x=offset.x, y=offset.y, z=offset.z)
        s += 'frame count {}\n'.format(len(self.__scene.frames))
        s += 'registered objects:\n'
        for ro in config.registered_objects:
            s += '\tname:{name}, id:{id}, type:{type}\n'.format(
                name=ro.name, id=ro.id, type=ro.type)
        s += 'config\n{}\n'.format(str(self.__o))

        return s

    def __read(self):
        with open(Path(self.__o.input, 'tracking.pb'), 'rb') as f:
            self.__scene.ParseFromString(f.read())


def main():
    options = parse_args()
    scene_processor = SceneProcessor(options)
    print(scene_processor)
    scene_processor.process()


if __name__ == '__main__':
    main()