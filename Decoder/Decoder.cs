using System;

namespace ImComp {
    class Decoder {
        private static byte[] UndoRle(byte[] compressed) {
            byte[] data = new byte[compressed.Length * 500];

            int dataPos = 0;

            for (var i = 0; i < compressed.Length; i += 4) {
                byte count = compressed[i + 3];

                for (var j = 0; j < count; j++) {
                    Array.Copy(compressed, i, data, dataPos, 3);

                    dataPos += 3;
                }
            }

            byte[] result = new byte[dataPos];

            Array.Copy(data, result, dataPos);

            return result;
        }

        public static byte[] DecodeImage(byte[] compressed, byte[] previous = null) {
            if (previous == null) {
                var rawData = UndoRle(compressed);

                byte[] data = new byte[(rawData.Length / 3) * 4];

                for (int i = 0; i < rawData.Length / 3; i++) {
                    Array.Copy(rawData, i * 3, data, i * 4, 3);

                    data[i * 4 + 3] = 255;
                }

                return data;
            }
            else {
                byte[] data = new byte[previous.Length];

                Array.Copy(previous, data, previous.Length);

                for (int i = 0; i < compressed.Length; i += 4) {
                    uint position = BitConverter.ToUInt32(compressed, i);

                    i += sizeof(uint);

                    byte count = compressed[i + 3];

                    for (var j = 0; j < count; ++j) {
                        Array.Copy(compressed, i, data, position * 4 + j * 4, 3);
                    }
                }

                return data;
            }
        }
    }
}
