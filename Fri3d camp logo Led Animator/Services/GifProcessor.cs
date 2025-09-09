namespace Fri3d_camp_logo_Led_Animator.Services;

using SixLabors.ImageSharp;
using SixLabors.ImageSharp.Processing;
using SixLabors.ImageSharp.PixelFormats;
using SixLabors.ImageSharp.Formats.Gif;
using System.IO;
using System.Collections.Generic;
using System.Linq;

/// <summary>
/// Represents a single LED with its X and Y coordinates.
/// </summary>
public struct LedPoint
{
    public int X;
    public int Y;
}

/// <summary>
/// A class to process a GIF and map its frames to a custom LED array.
/// </summary>
public class GifProcessor
{
    private readonly IWebHostEnvironment _env;

    // The constructor now accepts IWebHostEnvironment via dependency injection.
    public GifProcessor(IWebHostEnvironment env)
    {
        _env = env;
    }

    static int width = 1089;  // Target width for resizing
    static int height = 344; // Target height for resizing

    /// <summary>
    /// Processes a GIF and extracts pixel data for each LED.
    /// The LED map is loaded from a CSV file.
    /// </summary>
    /// <param name="gifFilePath">The file path to the GIF to process.</param>
    /// <param name="csvFilePath">The file path to the CSV containing the LED map.</param>
    /// <returns>A list of byte arrays, where each array holds the RGB data for one frame.</returns>
    public List<byte[]> ProcessGif(string gifFilePath)
    {
        // Construct the path to the CSV file using the ContentRootPath.
        string csvFilePath = Path.Combine(_env.ContentRootPath, "Documents", "fri3d led mapping.csv");

        // First, load the LED map from the shared CSV file.
        var ledMap = LoadLedMapFromCsv(csvFilePath);

        if (ledMap == null || ledMap.Count == 0)
        {
            System.Console.WriteLine("Error: The LED map could not be loaded or is empty.");
            return new List<byte[]>();
        }

        var processedFrames = new List<byte[]>();

        try
        {
            // Load the GIF file using ImageSharp.
            using (var gif = Image.Load(gifFilePath))
            {
                // Resize the entire GIF to the target dimensions before processing.
                gif.Mutate(x => x.Resize(width, height));

                // Iterate through each frame of the GIF.
                foreach (ImageFrame<Rgba32> frame in gif.Frames)
                {
                    // Create a byte array to store the RGB data for the current frame.
                    var frameBytes = new byte[ledMap.Count * 3];
                    int byteIndex = 0;


                    //// Iterate through the predefined LED map.
                    foreach (var point in ledMap)
                    {
                        try
                        {
                        // Get the pixel color at the specified (X, Y) coordinate.
                        var pixel = frame[point.X, point.Y];

                        // Add the R, G, and B values to the frame byte array.
                        frameBytes[byteIndex++] = pixel.R;
                        frameBytes[byteIndex++] = pixel.G;
                        frameBytes[byteIndex++] = pixel.B;
                        }
                        catch (Exception testex)
                        {

                            throw;
                        }

                    }

                    // Add the completed frame's byte array to our list.
                    processedFrames.Add(frameBytes);
                }
            }
        }
        catch (SixLabors.ImageSharp.UnknownImageFormatException)
        {
            System.Console.WriteLine($"Error: The file at {gifFilePath} is not a valid image format.");
        }
        catch (System.Exception ex)
        {
            System.Console.WriteLine($"An error occurred during GIF processing: {ex.Message}");
        }

        return processedFrames;
    }

    /// <summary>
    /// Loads the LED mapping from a CSV file.
    /// </summary>
    /// <param name="csvFilePath">The path to the CSV file.</param>
    /// <returns>A List of LedPoint structs.</returns>
    private List<LedPoint> LoadLedMapFromCsv(string csvFilePath)
    {
        var ledMap = new List<LedPoint>();
        try
        {
            var lines = File.ReadAllLines(csvFilePath);
            foreach (var line in lines)
            {
                var parts = line.Split(';');
                if (parts.Length >= 3)
                {
                    // The format is assumed to be: led_number;x_coord;y_coord
                    if (int.TryParse(parts[1].Trim(), out int x) && int.TryParse(parts[2].Trim(), out int y))
                    {
                        ledMap.Add(new LedPoint { X = x, Y = y });
                    }
                }
            }
        }
        catch (System.Exception ex)
        {
            System.Console.WriteLine($"Error loading LED map from CSV: {ex.Message}");
            return null;
        }
        return ledMap;
    }
}
