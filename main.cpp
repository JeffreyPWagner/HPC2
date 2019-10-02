#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <sstream>
#include <cstdlib>
#include "omp.h"

using namespace std;
using namespace std::chrono;
int main() {
    string version; // the pgm version of the input image
    int numCols = 0; // the wid
    int numRows = 0;
    int pixelMax = 0;

    // open files and reader
    ifstream imageFile("C:/Users/jeffp/CLionProjects/HPC2/yeast.pgm");
    ofstream output("C:/Users/jeffp/CLionProjects/HPC2/processedImage.pgm", std::ios_base::binary);
    stringstream ss;
    ss << imageFile.rdbuf();

    // get and print header info
    ss >> version;
    ss >> numCols >> numRows;
    ss >> pixelMax;

    cout << "Max threads: " << omp_get_max_threads() << "\n";
    cout << "Version : " << version << "\n" << numCols << " columns and " << numRows << " rows" << "\n";
    cout << "Max pixel value: " << pixelMax << "\n";

    // add rows and columns for padding
    numRows += 2;
    numCols += 2;

    // create array to hold pixel values of source image
    int** pixelArray = new int*[numRows];
    for(int i = 0; i < numRows; i++)
        pixelArray[i] = new int[numCols];

    // populate source image array
    for(int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            if (row == 0 || row == numRows - 1) {
                pixelArray[row][col] = 0;
            } else if (col == 0 || col == numCols - 1){
                pixelArray[row][col] = 0;
            } else {
                ss >> pixelArray[row][col];
            }
        }
    }

    // create array to hold pixel values of updated image
    int** updatedPixelArray = new int*[numRows];
    for(int i = 0; i < numRows; i++)
        updatedPixelArray[i] = new int[numCols];

    // start timer for image processing
    auto start = std::chrono::system_clock::now();

    // populate processed image array including 0 values for padding around outside edges
    #pragma omp parallel for shared(updatedPixelArray)
    for(int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            // add 0 value padding to first and last row/column
            if (row == 0 || row == numRows - 1) {
                updatedPixelArray[row][col] = 0;
            } else if (col == 0 || col == numCols - 1){
                updatedPixelArray[row][col] = 0;
            } else {
                // calculate new pixel value using Sobel operator and threshold
                double pixelVal =
                    abs(pixelArray[row - 1][col - 1]  +
                    2 * pixelArray[row][col - 1] +
                    pixelArray[row + 1][col - 1] -
                    pixelArray[row - 1][col + 1] -
                    2* pixelArray[row][col + 1] -
                    pixelArray[row + 1][col + 1]) +
                    abs(pixelArray[row - 1][col - 1] +
                    2 * pixelArray[row - 1][col] +
                    pixelArray[row - 1][col + 1] -
                    pixelArray[row + 1][col - 1] -
                    2 * pixelArray[row + 1][col] -
                    pixelArray[row + 1][col + 1]);
                pixelVal = pixelVal > pixelMax ? pixelMax : pixelVal;
                pixelVal = pixelVal < 0 ? 0 : pixelVal;
                updatedPixelArray[row][col] = int(pixelVal);
            }
        }
    }

    // end timer for image processing
    std::chrono::duration<double> duration = (std::chrono::system_clock::now() - start);

    // store the image header in our output file
    output << version << "\n";
    output << numCols << " " << numRows << "\r\n";
    output << pixelMax << "\r\n";

    // store the updated array in our output file
    for(int row = 0; row < numRows; ++row) {
        for(int col = 0; col < numCols; ++col) {
            output << updatedPixelArray[row][col] << " ";
        }
        output << "\r\n";
    }

    // flush the output
    output << std::flush;

    // delete the arrays from memory, close input/output files, and print timing result
    for(int i = 0; i < numRows; i++)
        delete[] pixelArray[i];
    delete[] pixelArray;

    for(int i = 0; i < numRows; i++)
        delete[] updatedPixelArray[i];
    delete[] updatedPixelArray;

    imageFile.close();
    output.close();

    cout << "This run took: " << duration.count() << " seconds";
}