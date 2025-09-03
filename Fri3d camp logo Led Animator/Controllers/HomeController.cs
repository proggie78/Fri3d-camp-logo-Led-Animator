using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Fri3d_camp_logo_Led_Animator.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Http;
using SixLabors.ImageSharp; // For ImageSharp types if needed

namespace Fri3d_camp_logo_Led_Animator.Controllers
{
    public class HomeController : Controller
    {
        private readonly ILogger<HomeController> _logger;

        public HomeController(ILogger<HomeController> logger)
        {
            _logger = logger;
        }

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
            try
            {
                // Save uploaded file to temp path
                using (var stream = new FileStream(tempFilePath, FileMode.Create))
                {
                    await uploadedFile.CopyToAsync(stream);
                }

                // Instantiate GifProcessor and process GIF
                //var processor = new GifProcessor();
                //var processedFrames = processor.ProcessGif(tempFilePath, 1, 378); // width=1, height=378

                // Convert List<byte[]> to List<string> (hex representation)
                var hexFrames = new List<string>();
                //foreach (var frame in processedFrames)
                //{
                //    hexFrames.Add("0x" + BitConverter.ToString(frame).Replace("-", ""));
                //}

                ViewData["HexFrames"] = hexFrames;
                return View("Index");
            }
            finally
            {
                // Ensure temp file is deleted
                if (System.IO.File.Exists(tempFilePath))
                {
                    System.IO.File.Delete(tempFilePath);
                }
            }
        }
    }
}
