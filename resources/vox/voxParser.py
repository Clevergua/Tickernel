import argparse
import struct
import os
import glob
from enum import Enum

class VoxModel:
    def __init__(self):
        self.size = ()
        self.voxels = []


class TickernelVoxelPropertyType(Enum):
    TICKERNEL_VOXEL_INT8 = 0
    TICKERNEL_VOXEL_UINT8 = 1
    TICKERNEL_VOXEL_INT16 = 2
    TICKERNEL_VOXEL_UINT16 = 3
    TICKERNEL_VOXEL_INT32 = 4
    TICKERNEL_VOXEL_UINT32 = 5
    TICKERNEL_VOXEL_FLOAT32 = 6

class TickernelVoxelModel:
    def __init__(self):
        self.propertyCount = 0
        self.names = []
        self.types = []
        self.vertexCount = 0
        self.indexToProperties = []
        import struct


def WriteTickernelVoxelModel(filePath, tickernelVoxelModel):
    with open(filePath, 'wb') as file:
        file.write(struct.pack('<I', tickernelVoxelModel.propertyCount))
        for name in tickernelVoxelModel. names:
            length = len(name) + 1
            file.write(struct.pack('<I', length))
            file.write(name.encode('utf-8') + b'\x00')
        
        for t in tickernelVoxelModel.types:
            file.write(struct.pack('<I', t.value))
        
        file.write(struct.pack('<I', tickernelVoxelModel.vertexCount))
        
        # for properties in tickernelVoxelModel.indexToProperties:
        for index, properties in enumerate(tickernelVoxelModel.indexToProperties):
            for property in properties:
                file.write(struct.pack('<B', property))
    
            
def ParseVoxFile(filePath):
    voxModels = []
    colors = []
    with open(filePath, 'rb') as file:
        magic = file.read(4)
        if magic != b'VOX ':
            raise ValueError("Not a valid vox file")
        
        version = struct.unpack('<I', file.read(4))[0]
        print(f"VOX version: {version}")

        mainChunkID = file.read(4)
        print(f"Main chunk ID: {mainChunkID}")
        if mainChunkID != b'MAIN':
            raise ValueError("Missing MAIN chunk")
        
        mainChunkContentBytes = struct.unpack('<I', file.read(4))[0]
        print(f"Num bytes of Main chunk content: {mainChunkContentBytes}")
        mainChunkChildrenBytes = struct.unpack('<I', file.read(4))[0]
        print(f"Num bytes of Main's children chunks: {mainChunkChildrenBytes}")
        # skip chunk countent
        file.seek(mainChunkContentBytes, 1)
        # handle children chunks
        while True:
            chunkID = file.read(4)
            if not chunkID:
                break
            chunkContentBytes = struct.unpack('<I', file.read(4))[0]
            # print(f"Num bytes of chunk content bytes: {chunkContentBytes}")
            chunkChildrenBytes = struct.unpack('<I', file.read(4))[0]
            # print(f"Num bytes of chunk children bytes: {chunkChildrenBytes}")
            if not chunkID:
                break
            else:
                if chunkID == b'SIZE':
                    print(f"Handle SIZE chunk")
                    x, y, z = struct.unpack('<III', file.read(12))
                    print(f"Model size: x={x}, y={y}, z={z}")
                    voxModels.append(VoxModel())
                    voxModels[len(voxModels) - 1].size = (x, y, z)
                elif chunkID == b'XYZI':
                    print(f"Handle XYZI chunk")
                    voxelCount = struct.unpack('<I', file.read(4))[0]
                    print(f"Num of voxels: {voxelCount}")
                    for _ in range(voxelCount):
                        x, y, z, color = struct.unpack('<BBBB', file.read(4))
                        voxModels[len(voxModels) - 1].voxels.append((x, y, z, color))
                        print(f'Voxel ({x}, {y}, {z}, {color})')
                elif chunkID == b'RGBA':
                    print(f"Handle RGBA chunk")
                    colors.append((0, 0, 0, 0))
                    colorCount = 255
                    for _ in range(colorCount):
                        r, g, b, a = struct.unpack('<BBBB', file.read(4))
                        colors.append((r, g, b, a))
                        print(f'Color ({r}, {g}, {b}, {a})')
                    file.seek(4, 1)
                else:
                    print(f"Skip unknown chunk with ID {chunkID}")
                    file.seek(chunkContentBytes, 1)
    tickernelVoxelModels = []
    print(len(voxModels))
    for voxModel in voxModels:
        tickernelVoxelModel = TickernelVoxelModel()
        tickernelVoxelModel.propertyCount = 7
        tickernelVoxelModel.names = ["x", "y", "z", "r", "g", "b", "a"]
        tickernelVoxelModel.types = [
            TickernelVoxelPropertyType.TICKERNEL_VOXEL_UINT8,
            TickernelVoxelPropertyType.TICKERNEL_VOXEL_UINT8,
            TickernelVoxelPropertyType.TICKERNEL_VOXEL_UINT8,
            TickernelVoxelPropertyType.TICKERNEL_VOXEL_UINT8,
            TickernelVoxelPropertyType.TICKERNEL_VOXEL_UINT8,
            TickernelVoxelPropertyType.TICKERNEL_VOXEL_UINT8,
            TickernelVoxelPropertyType.TICKERNEL_VOXEL_UINT8,
        ]
        vertexCount = len(voxModel.voxels)
        tickernelVoxelModel.vertexCount = vertexCount
        tickernelVoxelModel.indexToProperties = []
        for propertyIndex in range(tickernelVoxelModel.propertyCount):
            tickernelVoxelModel.indexToProperties.append([])
            
        for voxel in voxModel.voxels:
            tickernelVoxelModel.indexToProperties[0].append(voxel[0])
            tickernelVoxelModel.indexToProperties[1].append(voxel[1])
            tickernelVoxelModel.indexToProperties[2].append(voxel[2])
            colorIndex = voxel[3]
            color = colors[colorIndex]
            tickernelVoxelModel.indexToProperties[3].append(color[0])
            tickernelVoxelModel.indexToProperties[4].append(color[1])
            tickernelVoxelModel.indexToProperties[5].append(color[2])
            tickernelVoxelModel.indexToProperties[6].append(color[3])
            # print(f'tickernelVoxel: ({voxel[0]}, {voxel[1]}, {voxel[2]}) ({color[0]}, {color[1]}, {color[2]}, {color[3]})')
        tickernelVoxelModels.append(tickernelVoxelModel)

    directory = os.path.dirname(filePath)
    fileName = os.path.basename(filePath)
    name, ext = os.path.splitext(fileName)
    print(f"The directory of the file is: {directory}")
    for index, tickernelVoxelModel in enumerate(tickernelVoxelModels):
        print(f"Index: {index}, Model: {tickernelVoxelModel}")
        newFileName = f"{name}_{index}.tknvox"
        newFilePath = os.path.join(directory, newFileName)
        WriteTickernelVoxelModel(newFilePath, tickernelVoxelModel)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Parse .vox files in the specified directory.")
    parser.add_argument("directory_path", type=str, help="Path to the directory containing .vox files")
    args = parser.parse_args()

    # 使用 glob 模块查找目录中的所有 .vox 文件
    file_paths = glob.glob(os.path.join(args.directory_path, "*.vox"))

    for file_path in file_paths:
        print(f"Processing file: {file_path}")
        ParseVoxFile(file_path)


# if __name__ == "__main__":
#     parser = argparse.ArgumentParser(description="Parse .vox file and print vertices.")
#     parser.add_argument("file_path", type=str, help="Path to the .vox file")
#     args = parser.parse_args()
#     ParseVoxFile(args.file_path)