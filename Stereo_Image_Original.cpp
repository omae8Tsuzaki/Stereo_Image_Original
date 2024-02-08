#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace cv;
using namespace std;

//自作したstereo matchのプログラム(画像)

int main() {
	//ステレオマッチングのパラメータ指定
	int numDisparities = 32;
	int SADWindowSize = 7;//Block Size
	double UniquenessRatio = 15;//デフォルト15
	int SADthreshold = 2;//SADの閾値, デフォルト2
	////////文字列変換//////
	string D = to_string(numDisparities);
	string B = to_string(SADWindowSize);
	string U = to_string(UniquenessRatio);
	string T = to_string(SADthreshold);
	if (!(numDisparities%16 == 0)) {
		cout << "The value of numDisparities is wrong; enter a multiple of 16." << endl;
		return 1;
	}

	string filepath = "C:/Users/t-tsu/Documents/PROGRAM_FILE/workVS/Stereo_Image_Original"; 
	Mat input_L, input_R;
	input_L = imread(filepath + "./image/left.png");
	input_R = imread(filepath + "./image/right.png");

	if (input_L.empty() || input_R.empty()) {
		cout<< "File does not exist." << endl;
		return 2;
	}

	int width = input_L.cols;
	int height = input_L.rows;
	Mat gray_L = Mat(Size(width, height), CV_8UC1);
	Mat gray_R = Mat(Size(width, height), CV_8UC1);

	//テキストファイルに出力する視差データの指定する画像アドレス
	int w_sad = 320;
	int h_sad = 240;
	string W_SAD = to_string(w_sad);
	string H_SAD = to_string(h_sad);
	if (width <= w_sad || height <= h_sad) {
		std::cout << "The specified image coordinate value is incorrect. Please re-enter them." << endl;
		return 3;
	}
	/*
	//SADの値を出力するテキストファイル指定////////
	std::ofstream outputFile(filepath + "StereoOri_D" + D + "_B" + B + "_U" + U + "_T" + T + "_(" + W_SAD + "_" + H_SAD + ").txt");
	if (!outputFile) {
		std::cerr << "Failed to open the output file." << std::endl;
		return 4;
	}
	*/

	//出力先//////////////////////////////////////////
	Mat black = Mat::zeros(Size(width, height), CV_8UC3);
	black = Scalar(0, 0, 0);
	Mat dst = Mat(Size(width, height), CV_8UC1);//計算結果格納
	cvtColor(black, dst, COLOR_BGR2GRAY);
	Mat convert = Mat(Size(width, height), CV_8UC1);//正規化後
	double minVal, maxVal;
	///////////////////////////////////////////////////

	cvtColor(input_L, gray_L, COLOR_BGR2GRAY);
	cvtColor(input_R, gray_R, COLOR_BGR2GRAY);

	///////////////事前フィルタ/////////////////////////////////////////////////
	double before_L_minVal, before_L_maxVal, before_R_minVal, before_R_maxVal;
	minMaxLoc(gray_L, &before_L_minVal, &before_L_maxVal);
	gray_L.convertTo(gray_L, CV_8UC1, 255 / (before_L_maxVal - before_L_minVal));
	minMaxLoc(gray_R, &before_R_minVal, &before_R_maxVal);
	gray_R.convertTo(gray_R, CV_8UC1, 255 / (before_R_maxVal - before_R_minVal));
	/////////////////////////////////////////////////////////////////////////////

	for (int j = (SADWindowSize / 2) + 1; j <= gray_L.rows - SADWindowSize; j++) {
		for (int i_L = (SADWindowSize / 2) + 1; i_L <= gray_L.cols - SADWindowSize; i_L++) {
			Rect Rect_L(i_L, j, SADWindowSize, SADWindowSize);
			Mat copy_L = gray_L(Rect_L).clone();

			size_t index_1st = 0;
			std::vector<int> list;
			for (int D = 0; D < numDisparities; D++) {
				int i_R = i_L - D;
				if (i_R > 0) {
					Rect Rect_R(i_R, j, SADWindowSize, SADWindowSize);
					Mat copy_R = gray_R(Rect_R).clone();
					int SUM_SAD = 0;
					for (int y_R = 0; y_R < copy_R.rows; y_R++) {//右の画像のブロック
						for (int x_R = 0; x_R < copy_R.cols; x_R++) {
							SUM_SAD += abs(copy_L.at<unsigned char>(y_R, x_R) - copy_R.at<unsigned char>(y_R, x_R));
						}
					}
					list.push_back(SUM_SAD);
				}
				else if (i_R <= 0) {//左端のオクルージョン
					list.push_back(0);
				}
			}
			/*
			/////////画像座標を指定してlistの値をtextファイルに出力
			if (j == w_sad && i_L == h_sad) {
				for (size_t txt = 0; txt < list.size(); ++txt) {
					outputFile << txt << "," << list[txt] << endl;
				}
				std::cout << "D = " << D << ", Values written to the file successfully." << std::endl;
			}
			*/

			std::vector<int>::iterator iter_1 = std::min_element(list.begin(), list.end());//リスト内の最小の要素
			std::nth_element(list.begin(), list.begin() + 1, list.end());//２番目に小さい要素を見つける
			if ((list[0] < ((100 - UniquenessRatio) / 100) * list[1]) && (SADthreshold <= list[0])) {//listの最小要素が2番目に小さい要素の値の一定の割合未満かをチェック, listの最小要素が指定した閾値以上であることをチェック
				index_1st = std::distance(list.begin(), iter_1);
				dst.at<unsigned char>(j, i_L) = index_1st + 1;
			}
			else {
				dst.at<unsigned char>(j, i_L) = 0;
			}
		}
	}
	//正規化
	cv::minMaxLoc(dst, &minVal, &maxVal);
	dst.convertTo(convert, CV_8UC1, 255.0 / (numDisparities + 1));

	/*
	//////テキストファイルを閉じる
	outputFile.close();
	*/

	//imshow("L", input_L);
	//imshow("R", input_R);
	imshow("convert", convert);
	//imwrite(filepath + "StereoOri_D" + D + "_B" + B + "_U" + U + "_T" + T + ".jpg", convert);
	waitKey();

	return 0;
}