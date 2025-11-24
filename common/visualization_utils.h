#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

// Terminal Colors (for console output)
#ifndef RESET
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#endif

namespace viz
{

    // ============================================================================
    // Image Display & Window Management
    // ============================================================================

    /**
     * @brief Display a single image in a window
     * @param title Window title
     * @param img Image to display
     * @param wait_time Time to wait in milliseconds (0 = wait for key)
     */
    inline void showImage(const std::string &title, const cv::Mat &img, int wait_time = 0)
    {
        if (img.empty())
        {
            std::cerr << RED << "Error: Cannot display empty image" << RESET << std::endl;
            return;
        }
        cv::namedWindow(title, cv::WINDOW_AUTOSIZE);
        cv::imshow(title, img);
        if (wait_time >= 0)
        {
            cv::waitKey(wait_time);
        }
    }

    /**
     * @brief Display multiple images in a grid layout
     * @param titles Window titles for each image
     * @param images Vector of images to display
     * @param wait_time Time to wait in milliseconds (0 = wait for key)
     */
    inline void showImages(const std::vector<std::string> &titles,
                           const std::vector<cv::Mat> &images,
                           int wait_time = 0)
    {
        if (titles.size() != images.size())
        {
            std::cerr << RED << "Error: Number of titles and images must match" << RESET << std::endl;
            return;
        }

        for (size_t i = 0; i < images.size(); ++i)
        {
            if (!images[i].empty())
            {
                cv::namedWindow(titles[i], cv::WINDOW_AUTOSIZE);
                cv::imshow(titles[i], images[i]);
            }
        }

        if (wait_time >= 0)
        {
            cv::waitKey(wait_time);
        }
    }

    /**
     * @brief Create side-by-side comparison of two images with labels
     * @param img1 First image (e.g., CPU result)
     * @param img2 Second image (e.g., GPU result)
     * @param label1 Label for first image
     * @param label2 Label for second image
     * @return Combined image with labels
     */
    inline cv::Mat createComparison(const cv::Mat &img1, const cv::Mat &img2,
                                    const std::string &label1 = "Original",
                                    const std::string &label2 = "Processed")
    {
        if (img1.empty() || img2.empty())
        {
            std::cerr << RED << "Error: Cannot create comparison with empty images" << RESET << std::endl;
            return cv::Mat();
        }

        // Ensure images have the same dimensions
        cv::Mat img1_resized, img2_resized;
        if (img1.size() != img2.size())
        {
            cv::resize(img2, img2_resized, img1.size());
            img1_resized = img1.clone();
        }
        else
        {
            img1_resized = img1.clone();
            img2_resized = img2.clone();
        }

        // Convert to color if grayscale
        if (img1_resized.channels() == 1)
        {
            cv::cvtColor(img1_resized, img1_resized, cv::COLOR_GRAY2BGR);
        }
        if (img2_resized.channels() == 1)
        {
            cv::cvtColor(img2_resized, img2_resized, cv::COLOR_GRAY2BGR);
        }

        // Add text padding at top
        int text_height = 40;
        cv::Mat img1_with_label, img2_with_label;
        cv::copyMakeBorder(img1_resized, img1_with_label, text_height, 0, 0, 0,
                           cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
        cv::copyMakeBorder(img2_resized, img2_with_label, text_height, 0, 0, 0,
                           cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));

        // Add labels
        cv::putText(img1_with_label, label1, cv::Point(10, 25),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);
        cv::putText(img2_with_label, label2, cv::Point(10, 25),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);

        // Concatenate horizontally
        cv::Mat comparison;
        cv::hconcat(img1_with_label, img2_with_label, comparison);

        return comparison;
    }

    /**
     * @brief Create a grid of images with labels
     * @param images Vector of images to display
     * @param labels Vector of labels for each image
     * @param cols Number of columns in the grid
     * @return Combined image grid
     */
    inline cv::Mat createGrid(const std::vector<cv::Mat> &images,
                              const std::vector<std::string> &labels,
                              int cols = 2)
    {
        if (images.empty())
        {
            return cv::Mat();
        }

        int rows = (images.size() + cols - 1) / cols;
        int text_height = 40;

        // Find max dimensions
        int max_height = 0, max_width = 0;
        for (const auto &img : images)
        {
            max_height = std::max(max_height, img.rows);
            max_width = std::max(max_width, img.cols);
        }

        // Create grid
        std::vector<cv::Mat> row_images;
        for (int r = 0; r < rows; ++r)
        {
            std::vector<cv::Mat> col_images;

            for (int c = 0; c < cols; ++c)
            {
                int idx = r * cols + c;
                if (idx < static_cast<int>(images.size()))
                {
                    cv::Mat img_resized, img_color;

                    // Resize and convert to color
                    cv::resize(images[idx], img_resized, cv::Size(max_width, max_height));
                    if (img_resized.channels() == 1)
                    {
                        cv::cvtColor(img_resized, img_color, cv::COLOR_GRAY2BGR);
                    }
                    else
                    {
                        img_color = img_resized.clone();
                    }

                    // Add label
                    cv::Mat img_with_label;
                    cv::copyMakeBorder(img_color, img_with_label, text_height, 0, 0, 0,
                                       cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));

                    std::string label = idx < static_cast<int>(labels.size()) ? labels[idx] : "";
                    cv::putText(img_with_label, label, cv::Point(10, 25),
                                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 2);

                    col_images.push_back(img_with_label);
                }
                else
                {
                    // Empty placeholder
                    cv::Mat placeholder = cv::Mat::ones(max_height + text_height, max_width, CV_8UC3) * 255;
                    col_images.push_back(placeholder);
                }
            }

            cv::Mat row_concat;
            cv::hconcat(col_images, row_concat);
            row_images.push_back(row_concat);
        }

        cv::Mat grid;
        cv::vconcat(row_images, grid);
        return grid;
    }

    // ============================================================================
    // Difference & Error Visualization
    // ============================================================================

    /**
     * @brief Compute and visualize absolute difference between two images
     * @param img1 First image
     * @param img2 Second image
     * @param amplify Amplification factor for better visualization
     * @return Difference image (heatmap)
     */
    inline cv::Mat visualizeDifference(const cv::Mat &img1, const cv::Mat &img2,
                                       float amplify = 10.0f)
    {
        if (img1.size() != img2.size() || img1.type() != img2.type())
        {
            std::cerr << RED << "Error: Images must have same size and type" << RESET << std::endl;
            return cv::Mat();
        }

        cv::Mat diff, diff_abs, diff_amplified;
        cv::absdiff(img1, img2, diff_abs);

        // Amplify for visibility
        diff_abs.convertTo(diff_amplified, CV_32F);
        diff_amplified *= amplify;
        diff_amplified.convertTo(diff_abs, CV_8U);

        // Apply color map
        cv::Mat heatmap;
        cv::applyColorMap(diff_abs, heatmap, cv::COLORMAP_JET);

        return heatmap;
    }

    /**
     * @brief Calculate PSNR and MSE between two images
     * @param img1 First image
     * @param img2 Second image
     * @param psnr Output PSNR value
     * @param mse Output MSE value
     */
    inline void calculateImageQuality(const cv::Mat &img1, const cv::Mat &img2,
                                      double &psnr, double &mse)
    {
        if (img1.size() != img2.size() || img1.type() != img2.type())
        {
            std::cerr << RED << "Error: Images must have same size and type" << RESET << std::endl;
            psnr = 0.0;
            mse = -1.0;
            return;
        }

        cv::Mat diff;
        cv::absdiff(img1, img2, diff);
        diff.convertTo(diff, CV_32F);
        diff = diff.mul(diff);

        cv::Scalar s = cv::sum(diff);
        double sse = s[0] + s[1] + s[2];

        if (sse <= 1e-10)
        {
            psnr = 100.0;
            mse = 0.0;
        }
        else
        {
            mse = sse / (double)(img1.channels() * img1.total());
            psnr = 10.0 * std::log10((255.0 * 255.0) / mse);
        }
    }

    // ============================================================================
    // Feature & Keypoint Visualization
    // ============================================================================

    /**
     * @brief Draw keypoints on an image
     * @param img Input image
     * @param keypoints Vector of keypoints
     * @param color Color for drawing keypoints
     * @param draw_size Whether to draw circles proportional to keypoint size
     * @return Image with drawn keypoints
     */
    inline cv::Mat drawKeypoints(const cv::Mat &img,
                                 const std::vector<cv::KeyPoint> &keypoints,
                                 const cv::Scalar &color = cv::Scalar(0, 255, 0),
                                 bool draw_size = true)
    {
        cv::Mat output;
        if (img.channels() == 1)
        {
            cv::cvtColor(img, output, cv::COLOR_GRAY2BGR);
        }
        else
        {
            output = img.clone();
        }

        for (const auto &kp : keypoints)
        {
            int radius = draw_size ? std::max(1, static_cast<int>(kp.size / 2)) : 3;
            cv::circle(output, kp.pt, radius, color, 2);

            // Draw orientation if available
            if (kp.angle >= 0)
            {
                float angle_rad = kp.angle * CV_PI / 180.0f;
                cv::Point2f end(kp.pt.x + radius * std::cos(angle_rad),
                                kp.pt.y + radius * std::sin(angle_rad));
                cv::line(output, kp.pt, end, color, 1);
            }
        }

        return output;
    }

    /**
     * @brief Draw matches between two images
     * @param img1 First image
     * @param keypoints1 Keypoints from first image
     * @param img2 Second image
     * @param keypoints2 Keypoints from second image
     * @param matches Vector of matches
     * @param num_matches Number of matches to draw (-1 for all)
     * @return Combined image with matches drawn
     */
    inline cv::Mat drawMatches(const cv::Mat &img1,
                               const std::vector<cv::KeyPoint> &keypoints1,
                               const cv::Mat &img2,
                               const std::vector<cv::KeyPoint> &keypoints2,
                               const std::vector<cv::DMatch> &matches,
                               int num_matches = -1)
    {
        cv::Mat output;

        std::vector<cv::DMatch> matches_to_draw = matches;
        if (num_matches > 0 && num_matches < static_cast<int>(matches.size()))
        {
            matches_to_draw.resize(num_matches);
        }

        cv::drawMatches(img1, keypoints1, img2, keypoints2, matches_to_draw, output,
                        cv::Scalar::all(-1), cv::Scalar::all(-1),
                        std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        return output;
    }

    // ============================================================================
    // Performance Visualization
    // ============================================================================

    /**
     * @brief Create a performance comparison bar chart (as text overlay on image)
     * @param img Base image to overlay on
     * @param cpu_time CPU execution time in milliseconds
     * @param gpu_time GPU execution time in milliseconds
     * @param title Title for the chart
     * @return Image with performance overlay
     */
    inline cv::Mat addPerformanceOverlay(const cv::Mat &img,
                                         float cpu_time,
                                         float gpu_time,
                                         const std::string &title = "Performance")
    {
        cv::Mat output;
        if (img.channels() == 1)
        {
            cv::cvtColor(img, output, cv::COLOR_GRAY2BGR);
        }
        else
        {
            output = img.clone();
        }

        float speedup = cpu_time / gpu_time;

        // Create overlay rectangle
        int overlay_height = 120;
        cv::Mat overlay = cv::Mat::zeros(overlay_height, output.cols, CV_8UC3);
        overlay.setTo(cv::Scalar(40, 40, 40));

        // Add text
        int y_offset = 25;
        cv::putText(overlay, title, cv::Point(10, y_offset),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1);

        y_offset += 25;
        std::stringstream ss_cpu;
        ss_cpu << "CPU: " << std::fixed << std::setprecision(3) << cpu_time << " ms";
        cv::putText(overlay, ss_cpu.str(), cv::Point(10, y_offset),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(100, 100, 255), 1);

        y_offset += 25;
        std::stringstream ss_gpu;
        ss_gpu << "GPU: " << std::fixed << std::setprecision(3) << gpu_time << " ms";
        cv::putText(overlay, ss_gpu.str(), cv::Point(10, y_offset),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(100, 255, 100), 1);

        y_offset += 25;
        std::stringstream ss_speedup;
        ss_speedup << "Speedup: " << std::fixed << std::setprecision(2) << speedup << "x";
        cv::Scalar speedup_color = speedup > 1.0f ? cv::Scalar(100, 255, 255) : cv::Scalar(100, 100, 255);
        cv::putText(overlay, ss_speedup.str(), cv::Point(10, y_offset),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, speedup_color, 1);

        // Blend overlay onto image
        cv::Mat roi = output(cv::Rect(0, 0, output.cols, overlay_height));
        cv::addWeighted(roi, 0.3, overlay, 0.7, 0, roi);

        return output;
    }

    // ============================================================================
    // Utility Functions
    // ============================================================================

    /**
     * @brief Load image with error handling
     * @param filepath Path to image file
     * @param flags OpenCV imread flags (default: color)
     * @return Loaded image or empty Mat on failure
     */
    inline cv::Mat loadImage(const std::string &filepath, int flags = cv::IMREAD_COLOR)
    {
        cv::Mat img = cv::imread(filepath, flags);
        if (img.empty())
        {
            std::cerr << RED << "Error: Could not load image: " << filepath << RESET << std::endl;
        }
        else
        {
            std::cout << GREEN << "✓ Loaded: " << filepath
                      << " (" << img.cols << "x" << img.rows << ")" << RESET << std::endl;
        }
        return img;
    }

    /**
     * @brief Save image with error handling
     * @param filepath Path to save image
     * @param img Image to save
     * @return True if successful
     */
    inline bool saveImage(const std::string &filepath, const cv::Mat &img)
    {
        if (img.empty())
        {
            std::cerr << RED << "Error: Cannot save empty image" << RESET << std::endl;
            return false;
        }

        if (cv::imwrite(filepath, img))
        {
            std::cout << GREEN << "✓ Saved: " << filepath << RESET << std::endl;
            return true;
        }
        else
        {
            std::cerr << RED << "Error: Could not save image: " << filepath << RESET << std::endl;
            return false;
        }
    }

#ifdef __CUDACC__
    /**
     * @brief Convert CUDA array to cv::Mat
     * @param d_data Device pointer to image data
     * @param width Image width
     * @param height Image height
     * @param channels Number of channels
     * @param type OpenCV type (e.g., CV_8UC1)
     * @return cv::Mat with copied data
     * @note This function is only available when compiling with CUDA
     */
    inline cv::Mat cudaToMat(const void *d_data, int width, int height,
                             int channels, int type)
    {
        cv::Mat img(height, width, type);
        size_t size = width * height * channels * (type == CV_8UC1 || type == CV_8UC3 ? 1 : 4);
        cudaMemcpy(img.data, d_data, size, cudaMemcpyDeviceToHost);
        return img;
    }
#endif

    /**
     * @brief Print image statistics to console
     * @param img Input image
     * @param name Name/label for the image
     */
    inline void printImageStats(const cv::Mat &img, const std::string &name = "Image")
    {
        if (img.empty())
        {
            std::cout << RED << name << ": Empty image" << RESET << std::endl;
            return;
        }

        cv::Scalar mean, stddev;
        cv::meanStdDev(img, mean, stddev);

        double min_val, max_val;
        cv::minMaxLoc(img, &min_val, &max_val);

        std::cout << CYAN << name << " Statistics:" << RESET << std::endl;
        std::cout << "  Size: " << img.cols << "x" << img.rows
                  << " (" << img.channels() << " channels)" << std::endl;
        std::cout << "  Type: " << img.type() << std::endl;
        std::cout << "  Range: [" << min_val << ", " << max_val << "]" << std::endl;
        std::cout << "  Mean: " << mean[0];
        if (img.channels() > 1)
        {
            std::cout << ", " << mean[1] << ", " << mean[2];
        }
        std::cout << std::endl;
        std::cout << "  StdDev: " << stddev[0];
        if (img.channels() > 1)
        {
            std::cout << ", " << stddev[1] << ", " << stddev[2];
        }
        std::cout << std::endl;
    }

    /**
     * @brief Wait for key press and handle window cleanup
     * @param message Message to display
     * @return Key code pressed
     */
    inline int waitForKey(const std::string &message = "Press any key to continue (ESC to exit)...")
    {
        std::cout << YELLOW << message << RESET << std::endl;
        int key = cv::waitKey(0);
        if (key == 27)
        { // ESC key
            cv::destroyAllWindows();
        }
        return key;
    }

    /**
     * @brief Close all OpenCV windows
     */
    inline void closeAllWindows()
    {
        cv::destroyAllWindows();
    }

} // namespace viz
