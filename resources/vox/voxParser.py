#!/usr/bin/env python3
import argparse
import pyvox.models as voxel_models
import pyvox.parser as voxel_parser


def parse_vox_file(file_path):
    try:
        vox_data = voxel_parser.VoxParser(file_path).parse()
        for voxel in vox_data.models[0].voxels:
            print(f'Voxel at (x: {voxel.x}, y: {voxel.y}, z: {voxel.z})')
    except voxel_parser.ParsingException as e:
        print(f"Error parsing .vox file: {e}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Parse .vox file and print vertices.")
    parser.add_argument("file_path", type=str, help="Path to the .vox file")
    args = parser.parse_args()

    parse_vox_file(args.file_path)
