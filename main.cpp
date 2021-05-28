#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

const int nRange = 30;			// 나누는 구간 횟수
const int rotateDeg = 36;       // 회전 횟수
const int colorNum = 30;		// 색깔 수
int deg = 360 / rotateDeg;	// 횟수에 따른 회전 각
int nArray[nRange] = {};		// 히스토 그램 분포도 저장하기
int cnt = 0;					// 횟수 세기
int colorArr[nRange] = {};		// 3번째 위치들 저장하기

void thrNum(double arr[]) {			// 히스토그램에서 3번째로 큰 값 구하는 함수
	int k = 3, t;
	int bNum = 0;					// 3번재로 큰 색의 위치 저장 (0 ~ 29)
	double temp;

	for (int i = 0; i < k; i++) {
		t = i;
		for (int j = i; j < colorNum; j++) {
			if (arr[t] < arr[j]) {
				t = j;
				bNum = t;				// 위치 저장하기
			}
		}
		temp = arr[t];
		arr[t] = arr[i];
		arr[i] = temp;
	}
	colorArr[cnt] = bNum;				// 3번째로 큰 값을 배열에 저장하기
	cnt++;								
	bNum = 0;							// 값 초기화
}

void cHisto(Mat image, Mat& hist, int bins, int range_max = 256)
{
	hist = Mat(bins, 1, CV_32F, Scalar(0));
	float gap = range_max / (float)bins;

	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++)
		{
			int idx = int(image.at<uchar>(i, j) / gap);
			hist.at<float>(idx)++;
		}
	}
}

Mat  make_palatte(int rows)
{
	Mat hsv(rows, 1, CV_8UC3);
	for (int i = 0; i < rows; i++)
	{
		uchar hue = saturate_cast<uchar>((float)i / rows * 180);
		hsv.at<Vec3b>(i) = Vec3b(hue, 255, 255);
	}
	cvtColor(hsv, hsv, COLOR_HSV2BGR);
	return hsv;
}

void draw_histo_hue(Mat hist, Mat& hist_img, Size size = Size(256, 200))
{
	Mat hsv_palatte = make_palatte(hist.rows);

	hist_img = Mat(size, CV_8UC3, Scalar(255, 255, 255));
	float  bin = (float)hist_img.cols / hist.rows;
	double tmpArray[colorNum] = {};										// 히스토 분포 값 담을 배열
	normalize(hist, hist, 0, hist_img.rows, NORM_MINMAX);

	for (int i = 0; i < hist.rows; i++)
	{
		float start_x = (i * bin);
		float  end_x = (i + 1) * bin;
		Point2f pt1(start_x, 0);
		Point2f pt2(end_x, hist.at <float>(i));								
		Scalar color = hsv_palatte.at<Vec3b>(i);
		tmpArray[i] = hist.at <float>(i);								// 히스토 분포 값 담기
		if (pt2.y > 0) rectangle(hist_img, pt1, pt2, color, -1);	
	}
	thrNum(tmpArray);													// 3번째로 큰 값 저장하기
	
	flip(hist_img, hist_img, 0);
}
Mat rotateImg(Mat src, double angle)									// 이미지 회전하기
{
	Mat dst;      
	Point2f pt(src.cols / 2., src.rows / 2.);             
	Mat r = getRotationMatrix2D(pt, angle, 1.0);      
	warpAffine(src, dst, r, Size(src.cols, src.rows));  
	return dst;         
}

int main()
{
	Mat image = imread("C:\\Users\\poeynus\\Desktop\\openCV\\img\\6.jpg", IMREAD_COLOR);
	Mat gallery(500, 500, CV_8UC3, Scalar(255, 255, 255));				// 칼레이도 스코프 초기 직사각형 크기
	Mat kaleidoscope(800, 800, CV_8UC3, Scalar(255, 255, 255));			// 중첩할 배경
	CV_Assert(!image.empty());

	double alpha = 0.5;													// 이미지 블렌딩 변수 alpha
	double beta = 1.0 - alpha;											// 이미지 블렌딩 변수 beta
	int divImg = image.cols / nRange;									// 이미지 비율 나누기
	int divGry = gallery.cols / nRange;									// 칼레이도스코프의 초기 크기 나누기

	Scalar_<int> first(0, 0, 255), second(0, 51, 255), third(0, 102, 255), fourth(0, 153, 255),														// 30가지 색 지정
		fifth(0, 204, 255), sixth(0, 255, 255), seventh(0, 255, 204), eighth(0, 255, 153), ninth(0, 255, 102),
		tenth(0, 255, 51), eleventh(0, 255, 0), twelfth(51, 255, 0), thirteenth(102, 255, 0), fourteenth(153, 255, 0),
		fifteenth(204, 255, 0), sixteenth(255, 255, 0), seventeenth(255, 204, 0), eighteenth(255, 153, 0), nineteenth(255, 102, 0),
		twentieth(255, 51, 0), tFirst(255, 0, 0), tSecond(255, 0, 51), tThird(255, 0, 102), tFourth(255, 0, 153),
		tFifth(255, 0, 204), tSixth(255, 0, 255), tSeventh(204, 0, 255), tEighth(153, 0, 255), tNineth(102, 0, 255), thirtieth(51, 0, 255);

	for (int i = 0; i < nRange; i++) {
		Mat img = image(Rect(Point(divImg * i, 0), Point(divImg * (i + 1), image.rows)));
		Mat HSV_img, HSV_arr[3];
		cvtColor(img, HSV_img, COLOR_BGR2HSV);
		split(HSV_img, HSV_arr);

		Mat hue_hist, hue_hist_img;
		cHisto(HSV_arr[0], hue_hist, colorNum, 180);
		draw_histo_hue(hue_hist, hue_hist_img, Size(360, 200));
		//imshow(to_string(i+1), hue_hist_img);                     // 히스토 그램 분포도 확인
		//imshow(to_string(i + 100), img);							// 사진이 어떻게 나뉘어 졌는지 확인
	}

	// 히스토그램 분포도를 통해 사각형 그리기
	for (int i = 0; i < nRange; i++) {					
		switch (colorArr[i])
		{
		case 0:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2 , gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), first, FILLED);
			break;
		case 1:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), second, FILLED);
			break;
		case 2:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), third, FILLED);
			break;
		case 3:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), fourth, FILLED);
			break;
		case 4:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), fifth, FILLED);
			break;
		case 5:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), sixth, FILLED);
			break;
		case 6:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), seventh, FILLED);
			break;
		case 7:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), eighth, FILLED);
			break;
		case 8:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), ninth, FILLED);
			break;
		case 9:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tenth, FILLED);
			break;
		case 10:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), eleventh, FILLED);
			break;
		case 11:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), twelfth, FILLED);
			break;
		case 12:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), thirteenth, FILLED);
			break;
		case 13:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), fourteenth, FILLED);
			break;
		case 14:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), fifteenth, FILLED);
			break;
		case 15:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), sixteenth, FILLED);
			break;
		case 16:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), seventeenth, FILLED);
			break;
		case 17:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), eighteenth, FILLED);
			break;
		case 18:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), nineteenth, FILLED);
			break;
		case 19:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), twentieth, FILLED);
			break;
		case 20:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tFirst, FILLED);
			break;
		case 21:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tSecond, FILLED);
			break;
		case 22:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tThird, FILLED);
			break;
		case 23:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tFourth, FILLED);
			break;
		case 24:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tFifth, FILLED);
			break;
		case 25:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tSixth, FILLED);
			break;
		case 26:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tSeventh, FILLED);
			break;
		case 27:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tEighth, FILLED);
			break;
		case 28:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), tNineth, FILLED);
			break;
		case 29:
			rectangle(kaleidoscope, Point((divGry * i) + gallery.cols / 2, gallery.rows / 2), Point(divGry * (i + 1) + gallery.cols / 2, gallery.rows + gallery.rows / 2), thirtieth, FILLED);
			break;
		default:
			break;
		}
	}

	imshow("original", kaleidoscope);
	
	// 이미지 회전을 반복하고 이미지 블렌딩 중첩 처리하기
	for (int i = 1; i < rotateDeg+1; i++) {
		gallery = rotateImg(kaleidoscope, (double)i*deg);
		addWeighted(kaleidoscope, alpha, gallery, beta, 0.0, kaleidoscope);
	}
	
	imshow("Image", image);
	imshow("Kaleidoscope", kaleidoscope);
	waitKey();
	return 0;
}