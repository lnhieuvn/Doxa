// Δoxa Binarization Framework
// License: CC0 2018, "Freely you have received; freely give." - Matt 10:8
#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include "Image.hpp"
#include "Parameters.hpp"
#include "Palette.hpp"

namespace Doxa
{
	/// <summary>
	/// This is a base class for all of our binarization algorithms.
	/// It uses the Curiously Recurring Template Pattern for compile time inheritance.
	/// </summary>
	template<typename BinaryAlgorithm>
	class Algorithm
	{
	public:
		/// <summary>
		/// Sets the Gray Scale image that will later be used to generate a binary image.
		/// This allows the derived class to also initialize the image with any one time calculations.
		/// </summary>
		/// <param name="grayScaleImageIn">An Image object containing gray scale content</param>
		virtual void Initialize(const Image& grayScaleImageIn)
		{
			this->grayScaleImageIn = grayScaleImageIn.Reference();
		}

		/// <summary>
		/// Takes the initialized Gray Scale image and returns back a Binary image by reference.
		/// The Binary image memory should already be allocated before being passed by reference.
		/// This method was designed to be called repeatedly with different parameters.
		/// </summary>
		/// <param name="binaryImageOut">An Image object with preallocated memory which will store the output</param>
		/// <param name="parameters">Any parameters the algorithm may need</param>
		virtual void ToBinary(Image& binaryImageOut, const Parameters& parameters) = 0;

		/// <summary>
		/// A conveniance method for taking in a Gray Scale image /w params and returning a Binary image.
		/// </summary>
		static Image ToBinaryImage(const Image& grayScaleImageIn, const Parameters& parameters)
		{
			// Generate space for the binary image
			Image binaryImageOut(grayScaleImageIn.width, grayScaleImageIn.height);

			// Run Binarization Algorithm
			BinaryAlgorithm algorithm;
			algorithm.Initialize(grayScaleImageIn);
			algorithm.ToBinary(binaryImageOut, parameters);

			// The Move semantics allow this our underlying image to move without being copied
			return binaryImageOut;
		}

		/// <summary>
		/// A conveniance method for safely converting a Gray Scale image to Binary.
		/// Note: A temporary image is created to safely operate on, which may not always be necessary.
		/// </summary>
		static void UpdateToBinary(Image& image, const Parameters& parameters)
		{
			// Not all Calculators support in-place updating, thus we need a temporary Image store.
			Image binaryImageOut = ToBinaryImage(image, parameters);

			// Safely update our original memory location with the binary image data
			std::memcpy(image.data, binaryImageOut.data, binaryImageOut.size);
		}

	protected:
		Image grayScaleImageIn;
	};


	/// <summary>
	/// The base class for all Global Thresholding algorithms.
	/// </summary>
	template<typename BinaryAlgorithm>
	class GlobalThreshold : public Algorithm<BinaryAlgorithm>
	{
	public:
		/// <summary>
		/// Calculates and returns the global threshold of the image.
		/// </summary>
		/// <returns>A global binarization threshold value</returns>
		virtual Pixel8 Threshold(const Image& grayScaleImage, const Parameters& parameters) = 0;

		/// <summary>
		/// Global binarization based on a single threshold
		/// </summary>
		void ToBinary(Image& binaryImageOut, const Parameters& parameters)
		{
			const Pixel8 threshold = Threshold(Algorithm<BinaryAlgorithm>::grayScaleImageIn, parameters);

			for (int idx = 0; idx < Algorithm<BinaryAlgorithm>::grayScaleImageIn.size; ++idx)
			{
				binaryImageOut.data[idx] =
					Algorithm<BinaryAlgorithm>::grayScaleImageIn.data[idx] <= threshold ?
					Palette::Black : Palette::White;
			}
		}
	};
}


#endif //ALGORITHMS_HPP
