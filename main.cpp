#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
int main() {
    // note: most of the code to read in the file was adapted from a helpful stack overflow answer
    string line;
    int numCols = 0;
    int numRows = 0;
    int pixelMax = 0;
    stringstream ss;

    ifstream imageFile("C:/Users/jeffp/CLionProjects/HPC2/cell.pgm", std::ios_base::binary);

    ofstream output("C:/Users/jeffp/CLionProjects/HPC2/processedImage.pgm", std::ios_base::binary);

    getline(imageFile, line);
    if(line.compare("P2") != 0) cerr << "Version error" << endl;
    else cout << "Version : " << line << endl;

    ss << imageFile.rdbuf();
    ss >> numCols >> numRows;
    cout << numCols << " columns and " << numRows << " rows" << endl;

    ss >> pixelMax;
    cout << "Max pixel value: " << pixelMax << "\n";

    // add rows and columns for padding
    numRows += 2;
    numCols += 2;

    int** pixelArray = new int*[numRows];
    for(int i = 0; i < numRows; i++)
        pixelArray[i] = new int[numCols];

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

    int** updatedPixelArray = new int*[numRows];
    for(int i = 0; i < numRows; i++)
        updatedPixelArray[i] = new int[numCols];

    for(int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            if (row == 0 || row == numRows - 1) {
                updatedPixelArray[row][col] = 0;
            } else if (col == 0 || col == numCols - 1){
                updatedPixelArray[row][col] = 0;
            } else {
                double pixelVal =
                        pixelArray[row - 1][col - 1] * -0.0625
                        + pixelArray[row - 1][col] * -0.0625 +
                        pixelArray[row - 1][col + 1] * -0.0625 + pixelArray[row][col - 1] * -0.0625 +
                        pixelArray[row][col] * 0.5 +
                        pixelArray[row][col + 1] * -0.0625 + pixelArray[row + 1][col - 1] * -0.0625 +
                        pixelArray[row + 1][col] * -0.0625 +
                        pixelArray[row + 1][col + 1] * -0.0625;
                if (pixelVal < 2) {
                    updatedPixelArray[row][col] = 0;
                } else {
                    updatedPixelArray[row][col] = 255;
                }
            }
        }
    }

    cout << line;
    cout << numRows;
    cout << numCols;
    cout << pixelMax;

    output << line << "\r\n";
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
}