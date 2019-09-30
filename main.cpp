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
    // note: most of the code to read in the file was adapted from a helpful stack overflow answer
    string version;
    int numCols = 0;
    int numRows = 0;
    int pixelMax = 0;

    ifstream imageFile("C:/Users/jeffp/CLionProjects/HPC2/yeast.pgm");
    ofstream output("C:/Users/jeffp/CLionProjects/HPC2/processedImage.pgm", std::ios_base::binary);
    stringstream ss;
    ss << imageFile.rdbuf();

    ss >> version;
    ss >> numCols >> numRows;
    ss >> pixelMax;

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

    auto start = std::chrono::system_clock::now();

    // populate processed image array including 0 values for padding around outside edges
    #pragma omp parallel for shared(updatedPixelArray)
    for(int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            if (row == 0 || row == numRows - 1) {
                updatedPixelArray[row][col] = 0;
            } else if (col == 0 || col == numCols - 1){
                updatedPixelArray[row][col] = 0;
            } else {
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
                pixelVal = pixelVal > 70 ? 255 : 0;
                updatedPixelArray[row][col] = int(pixelVal);
            }
        }
    }

    std::chrono::duration<double> duration = (std::chrono::system_clock::now() - start);

    output << version << "\n";
    output << numCols << " " << numRows << "\r\n";
    output << pixelMax << "\r\n";

     // Now print the array to see the result
    for(int row = 0; row < numRows; ++row) {
        for(int col = 0; col < numCols; ++col) {
            output << updatedPixelArray[row][col] << " ";
        }
        output << "\r\n";
    }

    output << std::flush;

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