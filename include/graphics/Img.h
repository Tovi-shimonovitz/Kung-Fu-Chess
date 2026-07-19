#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>

class Img {
public:
    Img();
    
    /**
     * Load image from path and optionally resize.
     * 
     * @param path Image file to load
     * @param size Target size in pixels (width, height). If empty, keep original
     * @param keep_aspect If true, shrink so the longer side fits size while preserving aspect ratio
     * @param interpolation OpenCV interpolation flag (e.g., cv::INTER_AREA for shrink, cv::INTER_LINEAR for enlarge)
     * @return Reference to this object for method chaining
     */
    Img& read(const std::string& path,
              const std::pair<int, int>& size = {},
              bool keep_aspect = false,
              int interpolation = cv::INTER_AREA);

    /**
     * Return a resized copy of this image, leaving this image unchanged.
     *
     * @param width Target width in pixels
     * @param height Target height in pixels
     * @param interpolation OpenCV interpolation flag
     */
    Img resized(int width, int height, int interpolation = cv::INTER_LINEAR) const;

    /**
     * Create a blank (all-zero) image of the given size and channel count.
     *
     * @param width Width in pixels
     * @param height Height in pixels
     * @param channels Number of channels (e.g. 3 for BGR, 4 for BGRA)
     */
    static Img blank(int width, int height, int channels);

    /**
     * Return a deep copy of this image (independent pixel buffer).
     */
    Img clone() const;

    /**
     * Draw this image onto another image at position (x, y)
     * 
     * @param other_img The target image to draw on
     * @param x X coordinate for top-left corner
     * @param y Y coordinate for top-left corner
     */
    void draw_on(Img& other_img, int x, int y);

    /**
     * Zero out this image's pixels in place (no reallocation).
     */
    void clear();

    /**
     * Put text on the image
     * 
     * @param txt Text to draw
     * @param x X coordinate for text position
     * @param y Y coordinate for text position (baseline)
     * @param font_size Font scale factor
     * @param color Text color (BGR or BGRA)
     * @param thickness Text thickness
     */
    void put_text(const std::string& txt, int x, int y, double font_size,
                  const cv::Scalar& color = cv::Scalar(255, 255, 255, 255),
                  int thickness = 1);
    
    /**
     * Display the image in a window
     */
    void show();
    
    /**
     * Get the underlying OpenCV Mat
     */
    const cv::Mat& get_mat() const { return img; }
    
    /**
     * Check if image is loaded
     */
    bool is_loaded() const { return !img.empty(); }

private:
    cv::Mat img;
}; 