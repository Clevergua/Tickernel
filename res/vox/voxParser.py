import argparse
import math
import struct
import os
import glob
from enum import Enum

class VoxModel:
    def __init__(self):
        self.size = ()
        self.voxels = []


class TickernelVoxelPropertyType(Enum):
    TKN_VOXEL_INT8 = 0
    TKN_VOXEL_UINT8 = 1
    TKN_VOXEL_INT16 = 2
    TKN_VOXEL_UINT16 = 3
    TKN_VOXEL_INT32 = 4
    TKN_VOXEL_UINT32 = 5
    TKN_VOXEL_FLOAT32 = 6

class TickernelVoxelModel:
    def __init__(self):
        self.propertyCount = 0
        self.names = []
        self.types = []
        self.vertexCount = 0
        self.indexToProperties = []
        import struct

def SetNormals(voxModel, indexMap):
    vertexCount = len(voxModel.voxels)

    for i in range(vertexCount):
        x, y, z, colorIndex = voxModel.voxels[i]
        nx, ny, nz = 0, 0, 0
        if x - 1 in indexMap and y in indexMap[x - 1] and z in indexMap[x - 1][y]:
            nx += 1
        if x + 1 in indexMap and y in indexMap[x + 1] and z in indexMap[x + 1][y]:
            nx -= 1
        if x in indexMap and y - 1 in indexMap[x] and z in indexMap[x][y - 1]:
            ny += 1
        if x in indexMap and y + 1 in indexMap[x] and z in indexMap[x][y + 1]:
            ny -= 1
        if x in indexMap and y in indexMap[x] and z - 1 in indexMap[x][y]:
            nz += 1
        if x in indexMap and y in indexMap[x] and z + 1 in indexMap[x][y]:
            nz -= 1

        # length = 1 / 1.414
        # if x - 1 in indexMap and y - 1 in indexMap[x - 1] and z in indexMap[x - 1][y - 1]:
        #     nx += length
        #     ny += length
        # if x - 1 in indexMap and y + 1 in indexMap[x - 1] and z in indexMap[x - 1][y + 1]:
        #     nx += length
        #     ny -= length
        # if x - 1 in indexMap and y in indexMap[x - 1] and z - 1 in indexMap[x - 1][y]:
        #     nx += length
        #     nz += length
        # if x - 1 in indexMap and y in indexMap[x - 1] and z + 1 in indexMap[x - 1][y]:
        #     nx += length
        #     nz -= length
        # if x + 1 in indexMap and y - 1 in indexMap[x + 1] and z in indexMap[x + 1][y - 1]:
        #     nx -= length
        #     ny += length
        # if x + 1 in indexMap and y + 1 in indexMap[x + 1] and z in indexMap[x + 1][y + 1]:
        #     nx -= length
        #     ny -= length
        # if x + 1 in indexMap and y in indexMap[x + 1] and z - 1 in indexMap[x + 1][y]:
        #     nx -= length
        #     nz += length
        # if x + 1 in indexMap and y in indexMap[x + 1] and z + 1 in indexMap[x + 1][y]:
        #     nx -= length
        #     nz -= length
        # if x in indexMap and y - 1 in indexMap[x] and z - 1 in indexMap[x][y - 1]:
        #     ny += length
        #     nz += length
        # if x in indexMap and y - 1 in indexMap[x] and z + 1 in indexMap[x][y - 1]:
        #     ny += length
        #     nz -= length
        # if x in indexMap and y + 1 in indexMap[x] and z - 1 in indexMap[x][y + 1]:
        #     ny -= length
        #     nz += length
        # if x in indexMap and y + 1 in indexMap[x] and z + 1 in indexMap[x][y + 1]:
        #     ny -= length
        #     nz -= length
        # length = 1 / 1.414 / 1.414

        # if x - 1 in indexMap and y - 1 in indexMap[x - 1] and z - 1 in indexMap[x - 1][y - 1]:
        #     nx += length
        #     ny += length
        #     nz += length
        # if x - 1 in indexMap and y + 1 in indexMap[x - 1] and z - 1 in indexMap[x - 1][y + 1]:
        #     nx += length
        #     ny -= length
        #     nz += length
        # if x - 1 in indexMap and y - 1 in indexMap[x - 1] and z + 1 in indexMap[x - 1][y - 1]:
        #     nx += length
        #     ny += length
        #     nz -= length
        # if x - 1 in indexMap and y + 1 in indexMap[x - 1] and z + 1 in indexMap[x - 1][y + 1]:
        #     nx += length
        #     ny -= length
        #     nz -= length
        # if x + 1 in indexMap and y - 1 in indexMap[x + 1] and z - 1 in indexMap[x + 1][y - 1]:
        #     nx -= length
        #     ny += length
        #     nz += length
        # if x + 1 in indexMap and y + 1 in indexMap[x + 1] and z - 1 in indexMap[x + 1][y + 1]:
        #     nx -= length
        #     ny -= length
        #     nz += length
        # if x + 1 in indexMap and y - 1 in indexMap[x + 1] and z + 1 in indexMap[x + 1][y - 1]:
        #     nx -= length
        #     ny += length
        #     nz -= length
        # if x + 1 in indexMap and y + 1 in indexMap[x + 1] and z + 1 in indexMap[x + 1][y + 1]:
        #     nx -= length
        #     ny -= length
        #     nz -= length

        
        length = math.sqrt(nx * nx + ny * ny + nz * nz)
        if length > 0.001:
            nx = nx / length
            ny = ny / length
            nz = nz / length
        else:
            nx = 0
            ny = 0
            nz = 1
        voxModel.voxels[i] = (x, y, z, colorIndex, nx, ny, nz)

def WriteTickernelVoxelModel(filePath, tickernelVoxelModel):
    with open(filePath, 'wb') as file:
        file.write(struct.pack('<I', tickernelVoxelModel.propertyCount))
        for name in tickernelVoxelModel.names:
            length = len(name) + 1
            file.write(struct.pack('<I', length))
            file.write(name.encode('utf-8') + b'\x00')
        
        for t in tickernelVoxelModel.types:
            file.write(struct.pack('<I', t.value))
        
        file.write(struct.pack('<I', tickernelVoxelModel.vertexCount))
        
        for properties, t in zip(tickernelVoxelModel.indexToProperties, tickernelVoxelModel.types):
            for property in properties:
                if t == TickernelVoxelPropertyType.TKN_VOXEL_INT8:
                    file.write(struct.pack('<b', int(property)))
                elif t == TickernelVoxelPropertyType.TKN_VOXEL_UINT8:
                    file.write(struct.pack('<B', int(property)))
                elif t == TickernelVoxelPropertyType.TKN_VOXEL_INT16:
                    file.write(struct.pack('<h', int(property)))
                elif t == TickernelVoxelPropertyType.TKN_VOXEL_UINT16:
                    file.write(struct.pack('<H', int(property)))
                elif t == TickernelVoxelPropertyType.TKN_VOXEL_INT32:
                    file.write(struct.pack('<i', int(property)))
                elif t == TickernelVoxelPropertyType.TKN_VOXEL_UINT32:
                    file.write(struct.pack('<I', int(property)))
                elif t == TickernelVoxelPropertyType.TKN_VOXEL_FLOAT32:
                    file.write(struct.pack('<f', float(property)))
                else:
                    raise ValueError(f"Unsupported property type: {type(property)}")
                
def isSurrounded(x, y, z, indexMap):
    for dx in range(-1, 2):
        for dy in range(-1, 2):
            for dz in range(-1, 2):
                if dx == 0 and dy == 0 and dz == 0:
                    continue
                nx, ny, nz = x + dx, y + dy, z + dz
                if not (nx in indexMap and ny in indexMap[nx] and nz in indexMap[nx][ny]):
                    return False
    return True

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
        # skip chunk content
        file.seek(mainChunkContentBytes, 1)
        # handle children chunks
        while True:
            chunkID = file.read(4)
            if not chunkID:
                break
            chunkContentBytes = struct.unpack('<I', file.read(4))[0]
            chunkChildrenBytes = struct.unpack('<I', file.read(4))[0]
            if chunkID == b'SIZE':
                print(f"Handle SIZE chunk")
                x, y, z = struct.unpack('<III', file.read(12))
                print(f"Model size: x={x}, y={y}, z={z}")
                voxModels.append(VoxModel())
                voxModels[-1].size = (x, y, z)
            elif chunkID == b'XYZI':
                print(f"Handle XYZI chunk")
                voxelCount = struct.unpack('<I', file.read(4))[0]
                print(f"Num of voxels: {voxelCount}")
                for _ in range(voxelCount):
                    x, y, z, color = struct.unpack('<BBBB', file.read(4))
                    voxModels[-1].voxels.append((x, y, z, color))
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
    for voxModel in voxModels:
        tickernelVoxelModel = TickernelVoxelModel()
        tickernelVoxelModel.propertyCount = 10  # Including normals
        tickernelVoxelModel.names = ["px", "py", "pz", "r", "g", "b", "a", "nx", "ny", "nz"]
        tickernelVoxelModel.types = [
            TickernelVoxelPropertyType.TKN_VOXEL_UINT8,
            TickernelVoxelPropertyType.TKN_VOXEL_UINT8,
            TickernelVoxelPropertyType.TKN_VOXEL_UINT8,
            TickernelVoxelPropertyType.TKN_VOXEL_UINT8,
            TickernelVoxelPropertyType.TKN_VOXEL_UINT8,
            TickernelVoxelPropertyType.TKN_VOXEL_UINT8,
            TickernelVoxelPropertyType.TKN_VOXEL_UINT8,
            TickernelVoxelPropertyType.TKN_VOXEL_FLOAT32,
            TickernelVoxelPropertyType.TKN_VOXEL_FLOAT32,
            TickernelVoxelPropertyType.TKN_VOXEL_FLOAT32
        ]
        tickernelVoxelModel.vertexCount = 0
        tickernelVoxelModel.indexToProperties = [[] for _ in range(tickernelVoxelModel.propertyCount)]
        indexMap = {}
        vertexCount = len(voxModel.voxels)
        for i in range(vertexCount):
            x, y, z, colorIndex = voxModel.voxels[i]
            if x not in indexMap:
                indexMap[x] = {}
            if y not in indexMap[x]:
                indexMap[x][y] = {}
            indexMap[x][y][z] = i
        
        SetNormals(voxModel, indexMap)
        for voxel in voxModel.voxels:
            if(isSurrounded(voxel[0], voxel[1], voxel[2], indexMap)):
                continue
            tickernelVoxelModel.indexToProperties[0].append(voxel[0])
            tickernelVoxelModel.indexToProperties[1].append(voxel[1])
            tickernelVoxelModel.indexToProperties[2].append(voxel[2])
            colorIndex = voxel[3]
            color = colors[colorIndex]
            tickernelVoxelModel.indexToProperties[3].append(color[0])
            tickernelVoxelModel.indexToProperties[4].append(color[1])
            tickernelVoxelModel.indexToProperties[5].append(color[2])
            tickernelVoxelModel.indexToProperties[6].append(color[3])

            tickernelVoxelModel.indexToProperties[7].append(voxel[4])
            tickernelVoxelModel.indexToProperties[8].append(voxel[5])
            tickernelVoxelModel.indexToProperties[9].append(voxel[6])
            tickernelVoxelModel.vertexCount += 1
        tickernelVoxelModels.append(tickernelVoxelModel)

    directory = os.path.dirname(filePath)
    fileName = os.path.basename(filePath)
    name, ext = os.path.splitext(fileName)
    for index, tickernelVoxelModel in enumerate(tickernelVoxelModels):
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