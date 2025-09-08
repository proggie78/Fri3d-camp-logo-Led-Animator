// This file contains the complete Home Controller for the web application.
// It handles file uploads, processes the GIF, and generates a C++ header file for the ESP32.

using Microsoft.AspNetCore.Mvc;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Fri3d_camp_logo_Led_Animator.Models;
using Microsoft.AspNetCore.Http;
using Fri3d_camp_logo_Led_Animator.Services;
using System.Net.Mime;

namespace Fri3d_camp_logo_Led_Animator.Controllers;

public class HomeController(GifProcessor gifProcessor, ILogger<HomeController> logger, CppHeaderGenerator cppHeaderGenerator) : Controller
{
    private readonly GifProcessor _gifProcessor = gifProcessor;
    private readonly ILogger<HomeController> _logger = logger;
    private readonly CppHeaderGenerator _cppHeaderGenerator = cppHeaderGenerator;

    public IActionResult Index()
    {
        return View();
    }

    public IActionResult Visualizer()
    {
        return View();
    }

    [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
    public IActionResult Error()
    {
        return View(new ErrorViewModel { RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier });
    }

    [HttpPost]
    public async Task<IActionResult> ProcessGif(IFormFile uploadedFile)
    {
        if (uploadedFile == null || uploadedFile.Length == 0)
        {
            ViewData["Error"] = "No file uploaded or file is empty.";
            return View("Index");
        }

        string tempFilePath = Path.GetTempFileName();
        string outputHeaderFileName = "led_animation.h";
        string outputHeaderPath = Path.Combine(Path.GetTempPath(), outputHeaderFileName);

        try
        {
            // Save uploaded file to temp path
            await using (var stream = new FileStream(tempFilePath, FileMode.Create))
            {
                await uploadedFile.CopyToAsync(stream);
            }

            // Process the GIF to get the pixel data.
            var processedData = _gifProcessor.ProcessGif(tempFilePath);

            if (processedData.Count == 0)
            {
                ViewData["Error"] = "Could not process the GIF file. Please check the file format.";
                return View("Index");
            }

            // Generate the C++ header file with the processed data.
            _cppHeaderGenerator.GenerateHeaderFile(processedData, outputHeaderPath, "ledAnimationData");

            // Return the generated file as a downloadable file to the user.
            var fileBytes = await System.IO.File.ReadAllBytesAsync(outputHeaderPath);
            return File(fileBytes, MediaTypeNames.Text.Plain, outputHeaderFileName);
        }
        catch (System.Exception ex)
        {
            _logger.LogError(ex, "An error occurred during GIF processing.");
            ViewData["Error"] = "An error occurred during processing. Please try again.";
            return View("Index");
        }
        finally
        {
            // Ensure temp file is deleted
            if (System.IO.File.Exists(tempFilePath))
            {
                System.IO.File.Delete(tempFilePath);
            }
            if (System.IO.File.Exists(outputHeaderPath))
            {
                System.IO.File.Delete(outputHeaderPath);
            }
        }
    }
}
