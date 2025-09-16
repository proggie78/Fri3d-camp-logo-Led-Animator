using System.Collections.Generic;
using System.IO;

namespace Fri3d_camp_logo_Led_Animator.Services;

public class BinaryFileGenerator
{
    /// <summary>
    /// Generates a binary file with the animated pixel data.
    /// </summary>
    /// <param name="data">A list of byte arrays, where each array is a frame.</param>
    /// <param name="binaryFilePath">The full path where the binary file will be saved.</param>
    /// <param name="bw">Is the data black and white only</param>
    public void GenerateBinaryFile(List<byte[]> data, string binaryFilePath, bool bw)
    {
        using (var fs = new FileStream(binaryFilePath, FileMode.Create))
        {
            // Write the boolean value as the first byte (1 for true, 0 for false)
            fs.WriteByte(bw ? (byte)1 : (byte)0);

            // Loop through each frame and write the raw byte data
            foreach (var frame in data)
            {
                if (!bw)
                {
                    // For full color data, we write the full frame
                    fs.Write(frame, 0, frame.Length);
                }
                else
                {
                    // For black and white data, we pack 8 pixels into 1 byte to save space
                    int bitIndex = 0;
                    byte currentByte = 0;
                    for (int i = 0; i < frame.Length; i += 3)
                    {
                        // Check if the pixel is white (255) and set the bit
                        if (frame[i] == 255)
                        {
                            // Use a bitwise OR to set the correct bit in the byte
                            currentByte |= (byte)(1 << bitIndex);
                        }

                        // Move to the next bit
                        bitIndex++;

                        // If we have filled a full byte (8 bits) or reached the end of the frame, write the byte
                        if (bitIndex == 8 || i + 3 >= frame.Length)
                        {
                            fs.WriteByte(currentByte);
                            // Reset for the next byte
                            currentByte = 0;
                            bitIndex = 0;
                        }
                    }
                }
            }
        }
    }
}