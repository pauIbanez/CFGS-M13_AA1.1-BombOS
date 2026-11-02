#!/usr/bin/env node
// Reorders every skin's joints so parents come before their children.
// raylib only builds bone poses correctly when joints are topologically sorted.
// Usage: node scripts/sort-glb-joints.js <input.glb> [output.glb]

const fs = require('fs');

const [input, output = input] = process.argv.slice(2);
if (!input) {
  console.error('Usage: node scripts/sort-glb-joints.js <input.glb> [output.glb]');
  process.exit(1);
}

const glb = fs.readFileSync(input);
if (glb.readUInt32LE(0) !== 0x46546c67) throw new Error(`${input} is not a GLB file`);

const jsonLength = glb.readUInt32LE(12);
const gltf = JSON.parse(glb.subarray(20, 20 + jsonLength).toString('utf8'));
const binStart = 20 + jsonLength + 8;
const bin = glb.subarray(binStart, binStart + glb.readUInt32LE(20 + jsonLength));

const parents = new Map();
gltf.nodes.forEach((node, i) => (node.children || []).forEach((child) => parents.set(child, i)));

function accessorView(index) {
  const accessor = gltf.accessors[index];
  const view = gltf.bufferViews[accessor.bufferView];
  return { accessor, offset: (view.byteOffset || 0) + (accessor.byteOffset || 0), stride: view.byteStride };
}

const remappedAccessors = new Set();
let changed = false;

gltf.skins.forEach((skin, skinIndex) => {
  const joints = skin.joints;
  const isJoint = new Set(joints);

  // Depth first from the topmost joints keeps every parent ahead of its children
  const sorted = [];
  const visit = (node) => {
    if (isJoint.has(node)) sorted.push(node);
    (gltf.nodes[node].children || []).forEach(visit);
  };
  joints.filter((j) => !isJoint.has(parents.get(j))).forEach(visit);

  if (sorted.length !== joints.length) throw new Error(`Skin ${skinIndex} has joints outside its hierarchy`);
  if (sorted.every((node, i) => node === joints[i])) return;
  changed = true;

  const newIndex = new Map(sorted.map((node, i) => [node, i]));
  const oldToNew = joints.map((node) => newIndex.get(node));

  if (skin.inverseBindMatrices !== undefined) {
    const { offset } = accessorView(skin.inverseBindMatrices);
    const matrices = Buffer.from(bin.subarray(offset, offset + joints.length * 64));
    oldToNew.forEach((to, from) => matrices.copy(bin, offset + to * 64, from * 64, from * 64 + 64));
  }

  // Vertices store indices into the joint list, so point them at the new slots
  for (const mesh of gltf.meshes) {
    for (const primitive of mesh.primitives) {
      for (const [name, index] of Object.entries(primitive.attributes)) {
        if (!name.startsWith('JOINTS_') || remappedAccessors.has(index)) continue;
        remappedAccessors.add(index);

        const { accessor, offset, stride } = accessorView(index);
        const size = accessor.componentType === 5121 ? 1 : 2;
        const read = size === 1 ? (o) => bin.readUInt8(o) : (o) => bin.readUInt16LE(o);
        const write = size === 1 ? (v, o) => bin.writeUInt8(v, o) : (v, o) => bin.writeUInt16LE(v, o);

        for (let v = 0; v < accessor.count; v++) {
          const base = offset + v * (stride || size * 4);
          for (let c = 0; c < 4; c++) write(oldToNew[read(base + c * size)], base + c * size);
        }
      }
    }
  }

  skin.joints = sorted;
  console.log(`Skin ${skinIndex}: reordered ${joints.length} joints`);
});

if (!changed) {
  console.log('Joints already sorted, nothing to do');
  process.exit(0);
}

const json = Buffer.from(JSON.stringify(gltf), 'utf8');
const jsonPadded = Buffer.concat([json, Buffer.alloc((4 - (json.length % 4)) % 4, 0x20)]);

const header = Buffer.alloc(12);
header.writeUInt32LE(0x46546c67, 0);
header.writeUInt32LE(2, 4);
header.writeUInt32LE(12 + 8 + jsonPadded.length + 8 + bin.length, 8);

const chunk = (type, data) => {
  const head = Buffer.alloc(8);
  head.writeUInt32LE(data.length, 0);
  head.writeUInt32LE(type, 4);
  return Buffer.concat([head, data]);
};

fs.writeFileSync(output, Buffer.concat([header, chunk(0x4e4f534a, jsonPadded), chunk(0x004e4942, bin)]));
console.log(`Wrote ${output}`);
