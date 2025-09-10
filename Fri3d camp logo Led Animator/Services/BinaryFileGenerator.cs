namespace Fri3d_camp_logo_Led_Animator.Services;

// This class generates a binary file containing the animated pixel data
// for the ESP32 project.

using System.Collections.Generic;
using System.IO;

public class BinaryFileGenerator
{
    /// <summary>
    /// Generates a binary file with the animated pixel data.
    /// </summary>
    /// <param name="data">A list of byte arrays, where each array is a frame.</param>
    /// <param name="binaryFilePath">The full path where the binary file will be saved.</param>
    public void GenerateBinaryFile(List<byte[]> data, string binaryFilePath)
    {
        // Use a FileStream to write raw bytes to the file
        using (var fs = new FileStream(binaryFilePath, FileMode.Create))
        {
            // Loop through each frame and write the raw byte data
            foreach (var frame in data)
            {
                fs.Write(frame, 0, frame.Length);
            }
        }
    }
}
