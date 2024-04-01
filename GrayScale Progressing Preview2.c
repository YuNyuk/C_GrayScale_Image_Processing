#define _CRT_SECURE_NO_WARNINGS // scanf_s 안쓰려고
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <Windows.h> // GUI 지원 기능

//////////////////
// 전역 변수부
//////////////////
HWND hwnd; // 윈도우 화면(외부, 물리적)
HDC hdc; // 윈도우 화면(내부, 논리적)

FILE* rfp, * wfp;
char fileName[200];
int inH, inW, outH, outW;
int height;
int width;
// 메모리 확보
// unsigned char inImage[height][width], outImage[height][width];
unsigned char** inImage = NULL, ** outImage = NULL;

//////////////////
// 함수 선언부
//////////////////
void loadImage(); int getIntValue(); void printImage(); void printMenu();
void saveImage(); void freeInputMemory(); void freeOutputMemory();
void mallocInputMemory(); void mallocOutputMemory();
double** mallocDoubleMemory(int, int); void freeDoubleMemory(double**, int);

void equalImage(); void addImage(); void darkImage(); void grayImage();
void gammaImage(); void reverseImage(); void parabolCapImage(); void parabolCupImage();
void andImage(); void orImage(); void xorImage(); void zoomOut(); void zoomIn();
void zoomIn2(); void rotate(); void rotate2(); void moveImage(); void histoStretch();
void endIn(); void histoEqual(); void mirrorImage(); void emboss(); void blur();
void sharp(); void edge1(); void zoomIn3(); void rotateZoom1(); /*void rotateZoom2();*/



//////////////////
// 메인 함수부
//////////////////

void main() {
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(NULL); // Windows 10 : hwnd, Windows 11 : NULL

	char inKey = 0;
	while (inKey != '9') { // 메뉴 호출
		printMenu();
		inKey = _getch();
		system("cls");

		switch (inKey)
		{
		case '0': loadImage(); break;
		case '1': saveImage(); break;
		case '9': break;
		case 'q': case 'Q': equalImage(); break;
		case 'w': case 'W': addImage(); break;
		case 'e': case 'E': darkImage(); break;
		case 'r': case 'R': reverseImage(); break;
		case 't': case 'T': grayImage(); break;
		case 'u': case 'U': gammaImage(); break;
		case 'i': case 'I': parabolCapImage(); break;
		case 'o': case 'O': parabolCupImage(); break;
		case 'p': case 'P': andImage(); break;
		case '[': orImage(); break;
		case ']': xorImage(); break;
		case 'd': case 'D': zoomOut(); break;
		case 'a': case 'A': zoomIn(); break;
		case 's': case 'S': zoomIn2(); break;
		case 'g': case 'G': rotate(); break;
		case 'h': case 'H': rotate2(); break;
		case 'y': case 'Y': moveImage(); break;
		case 'n': case 'N': histoStretch(); break;
		case 'b': case 'B': endIn(); break;
		case 'm': case 'M': histoEqual(); break;
		case 'l': case 'L': mirrorImage(); break;
		case 'z': case 'Z': emboss(); break;
		case 'x': case 'X': blur(); break;
		case 'c': case 'C': sharp(); break;
		case 'v': case 'V': edge1(); break;
		case 'f': case 'F': zoomIn3(); break;
		case 'j': case 'J': rotateZoom1(); break;
		}
	}
	freeInputMemory();
	freeOutputMemory();
}


//////////////////
// 함수 정의부
//////////////////
// 공통 함수
/////////////

void printMenu() { // 메뉴 만들기
	puts("## GrayScale Image Processing (Beta 4) ##");
	puts("");
	puts("0.열기 1.저장 9.종료");
	puts("");
	puts("Q. 동일 / W.밝게 / E.어둡게 / R.반전 / T.흑백 / Y.이동");
	puts("U.감마 / I.파라볼라CAP / O.파라볼라CUP / P.AND 처리 / [.OR 처리 / ].XOR 처리");
	puts("A.확대(포워딩) / S.확대(백워딩) / D.축소 / F.확대(양선형 보간)");
	puts("G.회전 / H.회전(중앙, 백워딩) / J.회전+확대");
	puts("Z.엠보싱 / X.블러 / C.샤프닝 / V.경계선1 / B.엔드-인");
	puts("N.히스토그램 스트레칭 / M.평활화 / L.대칭");
}

void printImage() {
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			int px = outImage[i][k];
			SetPixel(hdc, k + 50, i + 250, RGB(px, px, px));
		}
	}
}

void loadImage() {
	char fullName[200] = "D:/RAW/Etc_Raw(squre)/";
	char tmpName[50];
	int isValidFileName = 0; // 파일명이 유효한지 여부를 나타내는 플래그

	while (!isValidFileName) {
		printf("파일명--> "); // ex) flower512, LENA256
		scanf("%s", tmpName);
		strcat(fullName, tmpName);
		strcat(fullName, ".raw");
		strcpy(fileName, fullName);
		// (중요!) 이미지의 폭과 높이를 결정
		rfp = fopen(fileName, "rb");
		if (rfp == NULL) {
			printf("잘못된 파일명입니다. 다시 입력해주세요.\n");
			strcpy(fullName, "D:/RAW/Etc_Raw(squre)/"); // fullName 초기화
			continue; // 다시 반복
		}

		fseek(rfp, 0L, SEEK_END); // 파일의 끝으로 이동
		long long fsize = ftell(rfp); // 파일 크기 계산
		fclose(rfp);

		//메모리 해제
		freeInputMemory();
		// 입력 영상 높이, 폭 구하기
		inH = inW = (int)sqrt(fsize);
		// 메모리 할당
		mallocInputMemory();

		// 파일 --> 메모리
		rfp = fopen(fileName, "rb");
		for (int i = 0; i < inH; i++)
			fread(inImage[i], sizeof(unsigned char), inW, rfp);
		fclose(rfp);

		isValidFileName = 1; // 파일명이 유효함을 표시
	}
	// printf("%d ", inImage[100][100]);

	equalImage();
}

void saveImage() {
	if (outImage == NULL)
		return;
	char fullName[200] = "D:/RAW/Etc_Raw(squre)/";
	char tmpName[50];
	printf("파일명--> "); // out01, out02...
	scanf("%s", tmpName);
	strcat(fullName, tmpName);
	strcat(fullName, ".raw");
	strcpy(fileName, fullName);

	wfp = fopen(fileName, "wb");
	// 메모리 --> 파일 (한 행씩)
	for (int i = 0; i < outH; i++)
		fwrite(outImage[i], sizeof(unsigned char), outW, wfp);
	fclose(wfp);
	MessageBox(hwnd, L"저장 완료", L"저장 창", NULL);
}

int getIntValue() {
	int retValue;
	printf("정수 값--> ");
	scanf("%d", &retValue);
	return retValue;
}

void freeInputMemory() {
	if (inImage == NULL)
		return;
	for (int i = 0; i < inH; i++)
		free(inImage[i]);
	free(inImage);
	inImage = NULL;
}

void mallocInputMemory() {
	inImage = (unsigned char**)malloc(sizeof(unsigned char*) * inH);
	for (int i = 0; i < inH; i++)
		inImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * inW);
}

void freeOutputMemory() {
	if (outImage == NULL)
		return;
	for (int i = 0; i < outH; i++)
		free(outImage[i]);
	free(outImage);
	outImage = NULL;
}

void mallocOutputMemory() {
	outImage = (unsigned char**)malloc(sizeof(unsigned char*) * outH);
	for (int i = 0; i < outH; i++)
		outImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * outW);
}

double** mallocDoubleMemory(int h, int w) {
	double** retMemory;
	retMemory = (double**)malloc(sizeof(double*) * h);
	for (int i = 0; i < h; i++)
		retMemory[i] = (double*)malloc(sizeof(double) * w);
	return retMemory;
}

void freeDoubleMemory(double** memory, int h) {
	if (memory == NULL)
		return;
	for (int i = 0; i < h; i++)
		free(memory[i]);
	free(memory);
	memory = NULL;
}

/////////////////
// 영상 처리 함수
/////////////////

void equalImage() { // 동일영상 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	// 입력 배열 --> 출력 배열
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[i][k];
		}
	}
	printf("원본 이미지 파일\n");
	printImage();
}

void addImage() { // 더하기(밝게) 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	int val = getIntValue();
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] + val < 255)
				outImage[i][k] = inImage[i][k] + val;
			else
				outImage[i][k] = 255;
		}
	}
	printf("밝게 처리\n");
	printImage();
}

void darkImage() { // 어둡게 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	// 입력 배열 --> 출력 배열
	int val = getIntValue();
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] - val > 0)
				outImage[i][k] = inImage[i][k] - val;
			else
				outImage[i][k] = 0;
		}
	}
	printf("어둡게 처리\n");
	printImage();
}

void grayImage() { // 흑백 처리 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] < 127)
				outImage[i][k] = 0;
			else
				outImage[i][k] = 255;
		}
	}
	printf("흑백 처리\n");
	printImage();
}

void reverseImage() { // 반전 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = 255 - inImage[i][k];
		}
	}
	printf("반전 처리\n");
	printImage();
}

void gammaImage() { // 감마 처리 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	float gamma;
	printf("감마 처리 (0~10) --> ");
	scanf("%f", &gamma); // 감마 값을 입력 받음

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			//픽셀 값 0~1로 정규화
			float normalized_pixel = (float)inImage[i][k] / 255.0;
			//감마 함수 적용후 보정된 값 계산
			float corrected_pixel = pow(normalized_pixel, gamma);

			corrected_pixel *= 255.0;

			if (corrected_pixel < 0)
				corrected_pixel = 0;
			if (corrected_pixel > 255)
				corrected_pixel = 255;

			outImage[i][k] = (unsigned char)corrected_pixel;
		}
	}
	printf("감마 처리\n");
	printImage();
}

void parabolCapImage() { // 파라볼라 Cap 처리 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	for (int i = 0; i < inH; i++)
	{
		for (int k = 0; k < inW; k++)
		{
			outImage[i][k] = 255 - 255 * pow((inImage[i][k] / 128 - 1), 2);
		}
	}
	printf("파라볼라 CAP 처리\n");
	printImage();
}

void parabolCupImage() { // 파라볼라 Cup 처리 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	for (int i = 0; i < inH; i++)
	{
		for (int k = 0; k < inW; k++)
		{
			outImage[i][k] = 255 * pow((inImage[i][k] / 128 - 1), 2);
		}
	}
	printf("파라볼라 CUP 처리\n");
	printImage();
}

void andImage() { // And 처리 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;
	int val = getIntValue();
	// 메모리 할당
	mallocOutputMemory();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if ((inImage[i][k] & (unsigned char)val) >= 255)
				outImage[i][k] = 255;
			else if ((inImage[i][k] & (unsigned char)val) < 0)
				outImage[i][k] = 0;
			else
				outImage[i][k] = (unsigned char)(inImage[i][k] & (unsigned char)val);
		}
	}
	printf("And 처리 알고리즘\n");
	printImage();
}

void orImage() { // Or 처리 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();
	int val = getIntValue();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if ((inImage[i][k] | (unsigned char)val) >= 255)
				outImage[i][k] = 255;
			else if ((inImage[i][k] | (unsigned char)val) < 0)
				outImage[i][k] = 0;
			else
				outImage[i][k] = (unsigned char)(inImage[i][k] | (unsigned char)val);
		}
	}
	printf("Or 처리 알고리즘\n");
	printImage();
}

void xorImage() { // XOR 처리 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	int val = getIntValue();
	// 입력 배열 --> 출력배열
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if ((inImage[i][k] ^ (unsigned char)val) >= 255)
				outImage[i][k] = 255;
			else if ((inImage[i][k] ^ (unsigned char)val) < 0)
				outImage[i][k] = 0;
			else
				outImage[i][k] = (unsigned char)(inImage[i][k] ^ (unsigned char)val);
		}
	}
	printf("XOR 처리 알고리즘\n");
	printImage();
}

void zoomOut() { // 축소 알고리즘
	int scale = getIntValue();
	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = (int)(inH / scale);
	outW = (int)(inW / scale);

	// 메모리 할당
	mallocOutputMemory();
	// 입력 배열 --> 출력 배열
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[(int)(i / scale)][(int)(k / scale)] = inImage[i][k];
		}
	}
	printf("축소\n");
	printImage();
}

void zoomIn() { // 포워딩 확대 알고리즘
	printf("배율 입력\n");
	int scale = getIntValue();
	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = (int)(inH * scale);
	outW = (int)(inW * scale);

	// 메모리 할당
	mallocOutputMemory();
	// 입력 배열 --> 출력 배열
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[(int)(i * scale)][(int)(k * scale)] = inImage[i][k];
		}
	}
	printf("확대(포워딩)\n");
	printImage();
}

void zoomIn2() { // 백워딩 확대 알고리즘
	printf("배율 입력\n");
	int scale = getIntValue();
	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = (int)(inH * scale);
	outW = (int)(inW * scale);

	// 메모리 할당
	mallocOutputMemory();
	// 입력 배열 --> 출력 배열
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[(int)(i / scale)][(int)(k / scale)];
		}
	}
	printf("확대(백워딩)\n");
	printImage();
}

void zoomIn3() { // 확대 (양선형 보간법)
	printf("배율 입력\n");
	int scale = getIntValue();
	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = (int)(inH * scale);
	outW = (int)(inW * scale);

	// 메모리 할당
	mallocOutputMemory();

	double rowRatio = (double)(inH - 1) / (outH - 1);
	double colRatio = (double)(inW - 1) / (outW - 1);

	// 출력 이미지의 각 픽셀에 대해 양선형 보간법 수행
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			// 현재 픽셀의 위치를 기준으로 입력 이미지에서 가장 가까운 네 개의 픽셀을 찾습니다.
			int baseRow = (int)(round(i * rowRatio));
			int baseCol = (int)(round(k * colRatio));

			// 현재 픽셀의 위치와 가장 가까운 네 개의 픽셀 사이의 거리를 계산합니다.
			double dx = (i * rowRatio) - baseRow;
			double dy = (k * colRatio) - baseCol;

			if (baseRow >= 0 && baseRow < inH - 1 && baseCol >= 0 && baseCol < inW - 1) {
				// 양선형 보간법을 사용하여 현재 픽셀의 값을 계산합니다.
				double interpolatedValue = (1 - dx) * (1 - dy) * inImage[baseRow][baseCol] +
					dx * (1 - dy) * inImage[baseRow + 1][baseCol] +
					(1 - dx) * dy * inImage[baseRow][baseCol + 1] +
					dx * dy * inImage[baseRow + 1][baseCol + 1];
				// 계산된 값으로 출력 이미지의 현재 픽셀을 설정합니다.
				outImage[i][k] = (unsigned char)interpolatedValue;
			}
			else {
				outImage[i][k] = 255;
			}
		}
	}
	printf("확대(양선형 보간)\n");
	printImage();
}

void rotate() { // 회전 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();
	int degree = getIntValue();
	double radian = degree * 3.141592 / 180.0;
	// xd = cos(xs) - sin(ys)
	// yd = sin(xs) + cos(ys)

	// 입력 배열 --> 출력 배열
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			int xs = i;
			int ys = k;
			int xd = cos(radian) * xs - sin(radian) * ys;
			int yd = sin(radian) * xs + cos(radian) * ys;

			if ((0 <= xd && xd < outH) && (0 <= yd && yd < outW))
				outImage[xd][yd] = inImage[xs][ys];
		}
	}
	printf("회전\n");
	printImage();
}

void rotate2() { // 회전(중앙, 백워딩) 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	// 입력 배열 --> 출력 배열
	int degree = getIntValue();
	double radian = degree * 3.141592 / 180.0;
	// xd = cos(xs) - sin(ys)
	// yd = sin(xs) + cos(ys)

	int cx = inH / 2;
	int cy = inW / 2;
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			int xd = i;
			int yd = k;
			int xs = (int)(cos(radian) * (xd - cx) + sin(radian) * (yd - cy));
			int ys = (int)(-sin(radian) * (xd - cx) + cos(radian) * (yd - cy));
			xs += cx;
			ys += cy;
			if ((0 <= xs && xs < outH) && (0 <= ys && ys < outW))
				outImage[xd][yd] = inImage[xs][ys];
		}
	}
	printf("회전 + 중심 + 백워딩\n");
	printImage();
}

void rotateZoom1() { // 회전+확대
	// 메모리 해제
	freeOutputMemory();

	// 회전 및 확대 각도 및 배율 입력
	printf("회전 각도 입력\n");
	int degree = getIntValue();
	printf("확대 배율 입력\n");
	int scale = getIntValue();
	double radian = -degree * 3.141592 / 180.0;

	// 회전 중심 계산
	int cx = inH / 2;
	int cy = inW / 2;

	// 출력 이미지 크기 계산
	outH = inH * scale;
	outW = inW * scale;

	// 메모리 할당
	mallocOutputMemory();

	// 회전 및 확대 알고리즘 수행
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			// 회전 및 확대된 픽셀 위치 계산
			int xd = i / scale;
			int yd = k / scale;
			int xs = (int)(cos(radian) * (xd - cx) - sin(radian) * (yd - cy) + cx);
			int ys = (int)(sin(radian) * (xd - cx) + cos(radian) * (yd - cy) + cy);

			// 회전된 픽셀 위치의 유효성 검사
			if (0 <= xs && xs < inH && 0 <= ys && ys < inW) {
				outImage[i][k] = inImage[xs][ys];
			}
			else {
				outImage[i][k] = 255; // 흰색으로 설정
			}
		}
	}

	printf("회전 및 확대 완료\n");
	printImage();
}

//void rotateZoom2() { // 회전+확대(양선형 보간)
//	freeOutputMemory();
//
//	// 회전 및 확대 각도 및 배율 입력
//	printf("회전 각도 입력\n");
//	int degree = getIntValue();
//	printf("확대 배율 입력\n");
//	int scale = getIntValue();
//	double radian = -degree * 3.141592 / 180.0;
//
//	// 회전 중심 계산
//	int cx = inH / 2; // 이미지의 높이를 기준으로 중심 계산
//	int cy = inW / 2; // 이미지의 너비를 기준으로 중심 계산
//
//	// 출력 이미지 크기 계산
//	outH = inH * scale;
//	outW = inW * scale;
//
//	// 메모리 할당
//	mallocOutputMemory();
//
//	double rowRatio = (double)(inH - 1) / (outH - 1);
//	double colRatio = (double)(inW - 1) / (outW - 1);
//
//	// 회전 및 확대 알고리즘 수행
//	for (int i = 0; i < outH; i++) {
//		for (int k = 0; k < outW; k++) {
//			// 회전 및 확대에 의해 변경된 좌표 계산
//			double xs = (cos(radian) * (i - cx) - sin(radian) * (k - cy)) / scale + cx;
//			double ys = (sin(radian) * (i - cx) + cos(radian) * (k - cy)) / scale + cy;
//
//			// 양선형 보간법을 사용하여 픽셀 값 계산
//			if (xs >= 0 && xs < inW - 1 && ys >= 0 && ys < inH - 1) {
//				int x1 = (int)xs;
//				int y1 = (int)ys;
//				int x2 = x1 + 1;
//				int y2 = y1 + 1;
//
//				double dx = xs - x1;
//				double dy = ys - y1;
//
//				// 양선형 보간법을 사용하여 현재 픽셀의 값을 계산
//				double interpolatedValue = (1 - dx) * (1 - dy) * inImage[y1][x1] +
//					dx * (1 - dy) * inImage[y1][x2] +
//					(1 - dx) * dy * inImage[y2][x1] +
//					dx * dy * inImage[y2][x2];
//
//				// 계산된 값으로 출력 이미지의 현재 픽셀 설정
//				outImage[i][k] = (unsigned char)interpolatedValue;
//			}
//			else {
//				outImage[i][k] = 255; // 범위를 벗어나는 경우 흰색으로 설정
//			}
//		}
//	}
//	printf("회전 및 확대 완료\n");
//	printImage();
//}

void histoStretch() { // 히스토그램 스트레칭 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	// 여기서부터 영상처리 알고리즘 시작~~
	// new = (old - low) / (high - low) * 255.0
	int high = inImage[0][0], low = inImage[0][0];
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] < low)
				low = inImage[i][k] < low;
			if (inImage[i][k] > high)
				high = inImage[i][k];
		}
	}
	int old, new;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			old = inImage[i][k];
			new = (int)((double)(old - low) / (double)(high - low) * 255.0);
			if (new > 255)
				new = 255;
			if (new < 0)
				new = 0;
			outImage[i][k] = new;
		}
	}
	printf("히스토그램 스트레칭\n");
	printImage();
}

void endIn() { // 엔드인 탐색 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	// 여기서부터 영상처리 알고리즘 시작~~
	// new = (old - low) / (high - low) * 255.0
	int high = inImage[0][0], low = inImage[0][0];
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] < low)
				low = inImage[i][k] < low;
			if (inImage[i][k] > high)
				high = inImage[i][k];
		}
	}

	high -= 50;
	low += 50;

	int old, new;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			old = inImage[i][k];
			new = (int)((double)(old - low) / (double)(high - low) * 255.0);
			if (new > 255)
				new = 255;
			if (new < 0)
				new = 0;
			outImage[i][k] = new;
		}
	}
	printf("엔드-인\n");
	printImage();
}

void histoEqual() { // 히스토그램 평활화 알고리즘

	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();


	// 1단계 : 빈도수 세기 (=히스토그램)
	int histo[256] = { 0. };
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			histo[inImage[i][k]]++;

	// 2단계 : 누적 히스토그램 생성
	int sumHisto[256] = { 0, };
	sumHisto[0] = histo[0];
	for (int i = 1; i < 256; i++)
		sumHisto[i] = sumHisto[i - 1] + histo[i];
	// 3단계 : 정규화된 히스토그램 생성 normalHisto = sumHisto * (1.0 / (inH*inW) * 255.0;
	double normalHisto[256] = { 1.0, };
	for (int i = 0; i < 256; i++) {
		normalHisto[i] = sumHisto[i] * (1.0 / (inH * inW)) * 255.0;
	}
	// 4단계 : inImage를 정규화된 값으로 치환
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = (unsigned char)normalHisto[inImage[i][k]];
		}
	}
	printf("히스토그램 평활화\n");
	printImage();
}

void moveImage() { // 이미지 이동 알고리즘
	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	int a = getIntValue();
	int b = getIntValue();
	int i, k = 0;
	// 입력 배열 --> 출력 배열
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			// 이미지 이동 시 경계를 벗어나는 경우를 고려하여 인덱스를 체크하여 처리
			if (i + a >= 0 && i + a < inH && k - b >= 0 && k - b < inW)
				outImage[i][k] = inImage[i + a][k - b];
			else 
				outImage[i][k] = 0; // 경계를 벗어나는 경우에는 0으로 처리하거나 다른 방법으로 처리
		}
	}
	printf("이미지 이동\n");
	printImage();
}

void mirrorImage() {
	printf("1 = 상하 반전 or 2 = 좌우 반전\n");
	// 메모리 해제
	freeOutputMemory();

	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;

	// 메모리 할당
	mallocOutputMemory();

	int a = getIntValue();
	// 1일때는 상하, 2일때는 좌우 반전

	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (a == 1)
				outImage[i][k] = inImage[inH - 1 - i][k]; // 상하 반전
			else
				outImage[i][k] = inImage[i][inW - 1 - k]; // 좌우 반전
		}
	}

	// 출력하는 문자열 선택
	char* mirrorType = (a == 1) ? "상하 대칭" : "좌우 대칭";
	printf("%s\n", mirrorType);

	printImage();
}

void emboss() { // 화소영역 처리 : 엠보싱 알고리즘

	// 메모리 해제
	freeOutputMemory();
	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;
	// 메모리 할당	
	mallocOutputMemory();

	////////////////
	// 화소 영역 처리
	/////////////////
	double mask[3][3] = 
	{ {-1.0, 0.0, 0.0}, // 엠보싱 마스크
	{ 0.0, 0.0, 0.0},
	{ 0.0, 0.0, 1.0} };
	// 임시 메모리 할당(실수형)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// 임시 입력 메모리를 초기화(127) : 필요시 평균값
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++) // 2 대신 1씩만 더해도 되긴 함
			tmpInImage[i][k] = 127;

	// 입력 이미지 --> 임시 입력 이미지
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** 회선 연산 ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// 마스크(3x3) 와 한점을 중심으로한 3x3을 곱하기
			S = 0.0; // 마스크 9개와 입력값 9개를 각각 곱해서 합한 값.
			for (int m = 0; m < 3; m++) {
				for (int n = 0; n < 3; n++) {
					S += tmpInImage[i + m][k + n] * mask[m][n];
				}
				tmpOutImage[i][k] = S;
			}
		}
	}
	// 후처리 (마스크 값의 합계에 따라서...)
	for (int i = 0; i < outH; i++)
		for (int k = 0; k < outW; k++)
			tmpOutImage[i][k] += 127.0;

	// 임시 출력 영상--> 출력 영상. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printf("엠보싱\n");
	printImage();
}

void blur() { // 화소영역 처리 : 블러링 알고리즘
	// 메모리 해제
	freeOutputMemory();
	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;
	// 메모리 할당	
	mallocOutputMemory();

	////////////////
	// 화소 영역 처리
	/////////////////
	double mask[3][3] = 
	{ {1. / 9, 1. / 9, 1. / 9}, // 블러링 마스크
	  {1. / 9, 1. / 9, 1. / 9},
	  {1. / 9, 1. / 9, 1. / 9} };
	// 임시 메모리 할당(실수형)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// 임시 입력 메모리를 초기화(127) : 필요시 평균값
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// 입력 이미지 --> 임시 입력 이미지
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** 회선 연산 ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// 마스크(3x3) 와 한점을 중심으로한 3x3을 곱하기
			S = 0.0; // 마스크 9개와 입력값 9개를 각각 곱해서 합한 값.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// 후처리 (마스크 값의 합계에 따라서...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// 임시 출력 영상--> 출력 영상. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printf("블러\n");
	printImage();
}

void sharp() { // 화소영역 처리 : 샤프닝 알고리즘
	// 메모리 해제
	freeOutputMemory();
	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;
	// 메모리 할당	
	mallocOutputMemory();

	////////////////
	// 화소 영역 처리
	/////////////////
	double mask[3][3] = 
	{ {0.0, -1.0 ,0.0}, // 샤프닝 마스크
	  {-1.0 ,5.0, -1.0},
	  {0.0, -1.0, 0.0} };
	// 임시 메모리 할당(실수형)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// 임시 입력 메모리를 초기화(127) : 필요시 평균값
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// 입력 이미지 --> 임시 입력 이미지
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** 회선 연산 ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// 마스크(3x3) 와 한점을 중심으로한 3x3을 곱하기
			S = 0.0; // 마스크 9개와 입력값 9개를 각각 곱해서 합한 값.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// 후처리 (마스크 값의 합계에 따라서...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// 임시 출력 영상--> 출력 영상. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printf("샤프닝\n");
	printImage();
}

void edge1() { // 경계선검출: 수직 에지 검출 마스크
	// 메모리 해제
	freeOutputMemory();
	// (중요!) 출력 이미지의 크기를 결정 ---> 알고리즘에 의존
	outH = inH;
	outW = inW;
	// 메모리 할당
	mallocOutputMemory();

	////////////////////
	// 화소 영역 처리
	////////////////////

	double mask[3][3] = { {0.0, 0.0, 0.0}, // 수직 에지 검출 마스크
						  {-1.0, 1.0, 0.0},
						  {0.0,  0.0, 0.0} };

	// 임시 메모리 할당(실수형)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// 임시 입력 메모리를 초기화(127) : 필요시 평균값 
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i][k] = 127;

	// 입력 이미지 --> 임시 입력 이미지
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// ** 회선 연산 ** 
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// 마스크(3x3)와 한점을 중심으로 한 3x3을 곱하기
			S = 0.0; // 마스크 9개와 입력값 9개를 각각 곱해서 합한 값.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// 후처리 (마스크 값의 합계에 따라서...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// 임시 출력 영상 --> 출력 영상. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printf("경계선 처리\n");
	printImage();
}

