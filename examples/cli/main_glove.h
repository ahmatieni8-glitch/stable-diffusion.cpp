
#pragma once

#define GLOVE_ENABLE_JSON
#include "glove.h"

glvm_SlvEnum(ProcessingMode, img_gen, vid_gen, convert);
glvm_SlvEnum_named(WeightType, weight_file_type, "", f32, "f32", f16, "f16", q4_0, "q4_0", q4_1, "q4_1", q5_0, "q5_0", q5_1, "q5_1", q8_0, "q8_0", q2_k, "q2_k", q3_k, "q3_k", q4_k, "q4_k");
glvm_SlvEnum_named(SamplingMethod, euler, "euler", euler_a, "euler_a", heun, "heun", dpm2, "dpm2", dpmpp2s_a, "dpm++2s_a", dpmpp2m, "dpm++2m", dpmpp2mv2, "dpm++2mv2", ipndm, "ipndm", ipndm_v, "ipndm_v", lcm, "lcm", ddim_trailing, "ddim_trailing", tcd, "tcd");
glvm_SlvEnum(Rng, std_default, cuda);
glvm_SlvEnum(Schedule, discrete, karras, exponential, ays, gits);

glvm_parametrization(GlvSDParamsPhotomaker, "Photomaker params",
                        stacked_id_embd_dir, SlvDirectory, "--stacked-id-embd-dir", "path to PHOTOMAKER stacked id embeddings", SlvDirectory(),
                        input_id_images_dir, SlvDirectory, "--input-id-images-dir", "path to PHOTOMAKER input id images dir", SlvDirectory(),
                        normalize_input, bool, "--normalize-input", "normalize PHOTOMAKER input id images", false);

glvm_parametrization(GlvSDParamsAdvanced, "Advanced params",
                        rng, Rng, "--rng", "RNG (default: cuda)", Rng::cuda,
                        threads, int, "--threads", "number of threads to use during computation (default: -1) \nIf threads <= 0, then threads will be set to the number of CPU physical cores", -1,
                        taesd, SlvFile, "--taesd", "path to taesd. Using Tiny AutoEncoder for fast decoding (low quality)", SlvFile(SlvFile::IO::Read),
                        control_net, SlvFile, "--control-net", "path to control net model", SlvFile(SlvFile::IO::Read),
                        embd_dir, SlvDirectory, "--embd-dir", "path to embeddings", SlvDirectory(),
                        upscale_model, SlvFile, "--upscale-model", "path to esrgan model. Upscale images after generate, just RealESRGAN_x4plus_anime_6B supported by now", SlvFile(SlvFile::IO::Read),
                        upscale_repeats, unsigned int, "--upscale-repeats", "Run the ESRGAN upscaler this many times (default 1)", 1,
                        type, WeightType, "--type", "weight type (examples: f32, f16, q4_0, q4_1, q5_0, q5_1, q8_0, q2_k, q3_k, q4_k) \nIf not specified, the default is the type of the weight file", WeightType::weight_file_type,
                        schedule, Schedule, "--schedule", "Denoiser sigma schedule (default: discrete)", Schedule::discrete,
                        clip_skip, int, "--clip-skip", "ignore last layers of CLIP network; 1 ignores none, 2 ignores one layer (default: -1) \n<= 0 represents unspecified, will be 1 for SD1.x, 2 for SD2.x", -1,
                        vae_tiling, bool, "--vae-tiling", "process vae in tiles to reduce memory usage", false,
                        vae_on_cpu, bool, "--vae-on-cpu", "keep vae in cpu (for low vram)", false,
                        clip_on_cpu, bool, "--clip-on-cpu", "keep clip in cpu (for low vram)", false,
                        diffusion_fa, bool, "--diffusion-fa", "use flash attention in the diffusion model (for low vram)\nMight lower quality, since it implies converting k and v to f16.\nThis might crash if it is not supported by the backend.", false,
                        control_net_cpu, bool, "--control-net-cpu", "keep controlnet in cpu (for low vram)", false,
                        canny, bool, "--canny", "apply canny preprocessor (edge detection)", false,
                        color, bool, "--color", "colors the logging tags according to level", false,
                        verbose, bool, "--verbose", "print extra info", false);

glvm_parametrization(GlvSDModelAddons, "SD model addons",
    clip_l, SlvFile, "--clip_l", "path to the clip-l text encoder", SlvFile(SlvFile::IO::Read),
    clip_g, SlvFile, "--clip_g", "path to the clip-g text encoder", SlvFile(SlvFile::IO::Read),
    t5xxl, SlvFile, "--t5xxl", "path to the the t5xxl text encoder", SlvFile(SlvFile::IO::Read),
    vae, SlvFile, "--vae", "path to vae", SlvFile("", SlvFileExtensions({".safetensors", ".sft"}), SlvFile::IO::Read),
    lora_model_dir, SlvDirectory, "--lora-model-dir", "lora model directory", SlvDirectory())

glvm_parametrization(GlvSDModelSLG, "SLG params",
    slg_scale, float, "--slg-scale", "skip layer guidance (SLG) scale, only for DiT models: (default: 0)\n0 means disabled, a value of 2.5 is nice for sd3.5 medium.", 0.f,
    skip_layers, std::vector<unsigned int>, "--skip-layers", "Layers to skip for SLG steps: (default: [7,8,9])", std::vector<unsigned int>({7, 8, 9}),
    skip_layer_start, float, "--skip-layer-start", "SLG enabling point: (default: 0.01)", 0.01f,
    skip_layer_end, float, "--skip-layer-end", "SLG disabling point: (default: 0.2)\nSLG will be enabled at step int([STEPS]*[START]) and disabled at int([STEPS]*[END])", 0.2f)

glvm_parametrization(GlvSDScaleStrength, "Scale/Strength params",
    cfg_scale, float, "--cfg-scale", "unconditional guidance scale: (default: 7.0)", 7.0f,
    guidance, float, "--guidance", "guidance scale for img2img (default: 3.5)", 3.5f,
    eta, float, "--eta", "eta in DDIM, only for DDIM and TCD: (default: 0)", 0.f,
    SLG, GlvSDModelSLG, "Skip layer guidance", "", GlvSDModelSLG(),
    strength, float, "--strength", "strength for noising/unnoising (default: 0.75)", 0.75f,
    style_ratio, SlvProportion, "--style-ratio", "strength for keeping input identity (default: 20%)", 0.2f,
    control_strength, SlvProportion, "--control-strength", "strength to apply Control Net (default: 0.9) \n1.0 corresponds to full destruction of information in init", 0.9f)

#ifdef SD_EXAMPLES_IMG2IMG_REPEAT
glvm_parametrization(GlvSDImagesSequence, "Images sequence params",
    crop, SlvSides2d<unsigned int>, "Crop", "Image crop at each frame", SlvSides2d<unsigned int>({2, 2, 2, 2}),
    Nframes, unsigned int, "Nframes", "Number of frames", 100)
#else
    typedef nullptr_t GlvSDAnim;
#endif

glvm_parametrization(GlvSDParams, "SD params",
    mode, ProcessingMode, "--mode", "run mode (txt2img or img2img or convert, default: txt2img)", ProcessingMode::img_gen,
    model, SlvFile, "--model", "path to full model", SlvFile("./", SlvFileExtensions({".safetensors", ".ckpt"}), SlvFile::IO::Read),
    diffusion_model, SlvFile, "--diffusion-model", "path to the standalone diffusion model", SlvFile(SlvFileExtensions({".gguf"}), SlvFile::IO::Read),
    model_addons, GlvSDModelAddons, "Model addons", "", GlvSDModelAddons(),
    photomaker_params, GlvSDParamsPhotomaker, "Photomaker", "", GlvSDParamsPhotomaker(),
    init_img, SlvFile, "--init-img", "path to the input image, required by img2img", SlvFile(SlvFile::IO::Read),
    mask_img, SlvFile, "--mask", "mask for inpainting", SlvFile(SlvFile::IO::Read),
    control_img, SlvFile, "--control-image", "path to image condition, control net", SlvFile(SlvFile::IO::Read),
    output, SlvFile, "--output", "path to write result image to (default: ./output.png)", SlvFile("./output.png", SlvFileExtensions({".png", ".jpg", ".jpeg", ".jpe"}), SlvFile::IO::Write),
    prompt, std::string, "--prompt", "the prompt to render", "", negative_prompt, std::string, "--negative-prompt", "the negative prompt (default: '')", "",
    scale_strength_params, GlvSDScaleStrength, "Scale/strength", "", GlvSDScaleStrength(),
    height, unsigned int, "--height", "image height, in pixel space (default: 512)", 512,
    width, unsigned int, "--width", "image width, in pixel space (default: 512)", 512,
    sampling_method, SamplingMethod, "--sampling-method", "{euler, euler_a, heun, dpm2, dpm++2s_a, dpm++2m, dpm++2mv2, ipndm, ipndm_v, lcm, ddim_trailing, tcd} \nsampling method (default: 'euler_a')", SamplingMethod::euler_a,
    steps, unsigned int, "--steps", "number of sample steps (default: 20)", 20,
    seed, int, "--seed", "RNG seed (default: 42, use random seed for < 0)", 42,
    batch_count, unsigned int, "--batch-count", "number of images to generate", 1,
    advanced_params, GlvSDParamsAdvanced, "Advanced", "", GlvSDParamsAdvanced(),
    images_sequence_params, GlvSDImagesSequence, "Images sequence", "Used only with img2img mode and if --init-img points to the same image as --output", GlvSDImagesSequence())

GLOVE_APP_CLI_PARAMETRIZATION_OUTPUT_DIRECTORY(GlvSDParams, "--output")


#ifdef SD_EXAMPLES_IMG2IMG_REPEAT
sd_image_t* crop(const sd_image_t& _image, int _left, int _right, int _up, int _bottom) {

    sd_image_t* image = new sd_image_t;

    image->width = _image.width - _left - _right;
    image->height  = _image.height - _up - _bottom;
    image->channel = _image.channel;
    image->data    = (uint8_t*)malloc(image->width * image->height * image->channel);

    uint8_t* data = _image.data;
    uint8_t* data2 = image->data;
    for (int m = 0; m < _image.width * _image.height * _image.channel; m++) {
    
        int j = (m - m % (_image.width * _image.channel)) / (_image.width * _image.channel);
        int n = m - j * _image.width * _image.channel;
        int i = (n - n % _image.channel) / (_image.channel);
        int k = n - i * _image.channel;

        bool l_ok = false;

        if (i >= _left) {
            if (i < _image.width - _right) {
                if (j >= _up) {
                    if (j < _image.height - _bottom) {
                        *data2 = *data;
                        data2++;
                        l_ok = true;
                    }
                }
            }
        }
        data++;
    }

    return image;
}
#endif
