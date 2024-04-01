#define _CRT_SECURE_NO_WARNINGS // scanf_s �Ⱦ�����
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <Windows.h> // GUI ���� ���

//////////////////
// ���� ������
//////////////////
HWND hwnd; // ������ ȭ��(�ܺ�, ������)
HDC hdc; // ������ ȭ��(����, ����)

FILE* rfp, * wfp;
char fileName[200];
int inH, inW, outH, outW;
int height;
int width;
// �޸� Ȯ��
// unsigned char inImage[height][width], outImage[height][width];
unsigned char** inImage = NULL, ** outImage = NULL;

//////////////////
// �Լ� �����
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
// ���� �Լ���
//////////////////

void main() {
	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(NULL); // Windows 10 : hwnd, Windows 11 : NULL

	char inKey = 0;
	while (inKey != '9') { // �޴� ȣ��
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
// �Լ� ���Ǻ�
//////////////////
// ���� �Լ�
/////////////

void printMenu() { // �޴� �����
	puts("## GrayScale Image Processing (Beta 4) ##");
	puts("");
	puts("0.���� 1.���� 9.����");
	puts("");
	puts("Q. ���� / W.��� / E.��Ӱ� / R.���� / T.��� / Y.�̵�");
	puts("U.���� / I.�Ķ󺼶�CAP / O.�Ķ󺼶�CUP / P.AND ó�� / [.OR ó�� / ].XOR ó��");
	puts("A.Ȯ��(������) / S.Ȯ��(�����) / D.��� / F.Ȯ��(�缱�� ����)");
	puts("G.ȸ�� / H.ȸ��(�߾�, �����) / J.ȸ��+Ȯ��");
	puts("Z.������ / X.�� / C.������ / V.��輱1 / B.����-��");
	puts("N.������׷� ��Ʈ��Ī / M.��Ȱȭ / L.��Ī");
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
	int isValidFileName = 0; // ���ϸ��� ��ȿ���� ���θ� ��Ÿ���� �÷���

	while (!isValidFileName) {
		printf("���ϸ�--> "); // ex) flower512, LENA256
		scanf("%s", tmpName);
		strcat(fullName, tmpName);
		strcat(fullName, ".raw");
		strcpy(fileName, fullName);
		// (�߿�!) �̹����� ���� ���̸� ����
		rfp = fopen(fileName, "rb");
		if (rfp == NULL) {
			printf("�߸��� ���ϸ��Դϴ�. �ٽ� �Է����ּ���.\n");
			strcpy(fullName, "D:/RAW/Etc_Raw(squre)/"); // fullName �ʱ�ȭ
			continue; // �ٽ� �ݺ�
		}

		fseek(rfp, 0L, SEEK_END); // ������ ������ �̵�
		long long fsize = ftell(rfp); // ���� ũ�� ���
		fclose(rfp);

		//�޸� ����
		freeInputMemory();
		// �Է� ���� ����, �� ���ϱ�
		inH = inW = (int)sqrt(fsize);
		// �޸� �Ҵ�
		mallocInputMemory();

		// ���� --> �޸�
		rfp = fopen(fileName, "rb");
		for (int i = 0; i < inH; i++)
			fread(inImage[i], sizeof(unsigned char), inW, rfp);
		fclose(rfp);

		isValidFileName = 1; // ���ϸ��� ��ȿ���� ǥ��
	}
	// printf("%d ", inImage[100][100]);

	equalImage();
}

void saveImage() {
	if (outImage == NULL)
		return;
	char fullName[200] = "D:/RAW/Etc_Raw(squre)/";
	char tmpName[50];
	printf("���ϸ�--> "); // out01, out02...
	scanf("%s", tmpName);
	strcat(fullName, tmpName);
	strcat(fullName, ".raw");
	strcpy(fileName, fullName);

	wfp = fopen(fileName, "wb");
	// �޸� --> ���� (�� �྿)
	for (int i = 0; i < outH; i++)
		fwrite(outImage[i], sizeof(unsigned char), outW, wfp);
	fclose(wfp);
	MessageBox(hwnd, L"���� �Ϸ�", L"���� â", NULL);
}

int getIntValue() {
	int retValue;
	printf("���� ��--> ");
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
// ���� ó�� �Լ�
/////////////////

void equalImage() { // ���Ͽ��� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	// �Է� �迭 --> ��� �迭
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[i][k];
		}
	}
	printf("���� �̹��� ����\n");
	printImage();
}

void addImage() { // ���ϱ�(���) �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
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
	printf("��� ó��\n");
	printImage();
}

void darkImage() { // ��Ӱ� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	// �Է� �迭 --> ��� �迭
	int val = getIntValue();
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] - val > 0)
				outImage[i][k] = inImage[i][k] - val;
			else
				outImage[i][k] = 0;
		}
	}
	printf("��Ӱ� ó��\n");
	printImage();
}

void grayImage() { // ��� ó�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] < 127)
				outImage[i][k] = 0;
			else
				outImage[i][k] = 255;
		}
	}
	printf("��� ó��\n");
	printImage();
}

void reverseImage() { // ���� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = 255 - inImage[i][k];
		}
	}
	printf("���� ó��\n");
	printImage();
}

void gammaImage() { // ���� ó�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	float gamma;
	printf("���� ó�� (0~10) --> ");
	scanf("%f", &gamma); // ���� ���� �Է� ����

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			//�ȼ� �� 0~1�� ����ȭ
			float normalized_pixel = (float)inImage[i][k] / 255.0;
			//���� �Լ� ������ ������ �� ���
			float corrected_pixel = pow(normalized_pixel, gamma);

			corrected_pixel *= 255.0;

			if (corrected_pixel < 0)
				corrected_pixel = 0;
			if (corrected_pixel > 255)
				corrected_pixel = 255;

			outImage[i][k] = (unsigned char)corrected_pixel;
		}
	}
	printf("���� ó��\n");
	printImage();
}

void parabolCapImage() { // �Ķ󺼶� Cap ó�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	for (int i = 0; i < inH; i++)
	{
		for (int k = 0; k < inW; k++)
		{
			outImage[i][k] = 255 - 255 * pow((inImage[i][k] / 128 - 1), 2);
		}
	}
	printf("�Ķ󺼶� CAP ó��\n");
	printImage();
}

void parabolCupImage() { // �Ķ󺼶� Cup ó�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	for (int i = 0; i < inH; i++)
	{
		for (int k = 0; k < inW; k++)
		{
			outImage[i][k] = 255 * pow((inImage[i][k] / 128 - 1), 2);
		}
	}
	printf("�Ķ󺼶� CUP ó��\n");
	printImage();
}

void andImage() { // And ó�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	int val = getIntValue();
	// �޸� �Ҵ�
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
	printf("And ó�� �˰���\n");
	printImage();
}

void orImage() { // Or ó�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
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
	printf("Or ó�� �˰���\n");
	printImage();
}

void xorImage() { // XOR ó�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	int val = getIntValue();
	// �Է� �迭 --> ��¹迭
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
	printf("XOR ó�� �˰���\n");
	printImage();
}

void zoomOut() { // ��� �˰���
	int scale = getIntValue();
	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = (int)(inH / scale);
	outW = (int)(inW / scale);

	// �޸� �Ҵ�
	mallocOutputMemory();
	// �Է� �迭 --> ��� �迭
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[(int)(i / scale)][(int)(k / scale)] = inImage[i][k];
		}
	}
	printf("���\n");
	printImage();
}

void zoomIn() { // ������ Ȯ�� �˰���
	printf("���� �Է�\n");
	int scale = getIntValue();
	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = (int)(inH * scale);
	outW = (int)(inW * scale);

	// �޸� �Ҵ�
	mallocOutputMemory();
	// �Է� �迭 --> ��� �迭
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[(int)(i * scale)][(int)(k * scale)] = inImage[i][k];
		}
	}
	printf("Ȯ��(������)\n");
	printImage();
}

void zoomIn2() { // ����� Ȯ�� �˰���
	printf("���� �Է�\n");
	int scale = getIntValue();
	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = (int)(inH * scale);
	outW = (int)(inW * scale);

	// �޸� �Ҵ�
	mallocOutputMemory();
	// �Է� �迭 --> ��� �迭
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[(int)(i / scale)][(int)(k / scale)];
		}
	}
	printf("Ȯ��(�����)\n");
	printImage();
}

void zoomIn3() { // Ȯ�� (�缱�� ������)
	printf("���� �Է�\n");
	int scale = getIntValue();
	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = (int)(inH * scale);
	outW = (int)(inW * scale);

	// �޸� �Ҵ�
	mallocOutputMemory();

	double rowRatio = (double)(inH - 1) / (outH - 1);
	double colRatio = (double)(inW - 1) / (outW - 1);

	// ��� �̹����� �� �ȼ��� ���� �缱�� ������ ����
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			// ���� �ȼ��� ��ġ�� �������� �Է� �̹������� ���� ����� �� ���� �ȼ��� ã���ϴ�.
			int baseRow = (int)(round(i * rowRatio));
			int baseCol = (int)(round(k * colRatio));

			// ���� �ȼ��� ��ġ�� ���� ����� �� ���� �ȼ� ������ �Ÿ��� ����մϴ�.
			double dx = (i * rowRatio) - baseRow;
			double dy = (k * colRatio) - baseCol;

			if (baseRow >= 0 && baseRow < inH - 1 && baseCol >= 0 && baseCol < inW - 1) {
				// �缱�� �������� ����Ͽ� ���� �ȼ��� ���� ����մϴ�.
				double interpolatedValue = (1 - dx) * (1 - dy) * inImage[baseRow][baseCol] +
					dx * (1 - dy) * inImage[baseRow + 1][baseCol] +
					(1 - dx) * dy * inImage[baseRow][baseCol + 1] +
					dx * dy * inImage[baseRow + 1][baseCol + 1];
				// ���� ������ ��� �̹����� ���� �ȼ��� �����մϴ�.
				outImage[i][k] = (unsigned char)interpolatedValue;
			}
			else {
				outImage[i][k] = 255;
			}
		}
	}
	printf("Ȯ��(�缱�� ����)\n");
	printImage();
}

void rotate() { // ȸ�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();
	int degree = getIntValue();
	double radian = degree * 3.141592 / 180.0;
	// xd = cos(xs) - sin(ys)
	// yd = sin(xs) + cos(ys)

	// �Է� �迭 --> ��� �迭
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
	printf("ȸ��\n");
	printImage();
}

void rotate2() { // ȸ��(�߾�, �����) �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	// �Է� �迭 --> ��� �迭
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
	printf("ȸ�� + �߽� + �����\n");
	printImage();
}

void rotateZoom1() { // ȸ��+Ȯ��
	// �޸� ����
	freeOutputMemory();

	// ȸ�� �� Ȯ�� ���� �� ���� �Է�
	printf("ȸ�� ���� �Է�\n");
	int degree = getIntValue();
	printf("Ȯ�� ���� �Է�\n");
	int scale = getIntValue();
	double radian = -degree * 3.141592 / 180.0;

	// ȸ�� �߽� ���
	int cx = inH / 2;
	int cy = inW / 2;

	// ��� �̹��� ũ�� ���
	outH = inH * scale;
	outW = inW * scale;

	// �޸� �Ҵ�
	mallocOutputMemory();

	// ȸ�� �� Ȯ�� �˰��� ����
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			// ȸ�� �� Ȯ��� �ȼ� ��ġ ���
			int xd = i / scale;
			int yd = k / scale;
			int xs = (int)(cos(radian) * (xd - cx) - sin(radian) * (yd - cy) + cx);
			int ys = (int)(sin(radian) * (xd - cx) + cos(radian) * (yd - cy) + cy);

			// ȸ���� �ȼ� ��ġ�� ��ȿ�� �˻�
			if (0 <= xs && xs < inH && 0 <= ys && ys < inW) {
				outImage[i][k] = inImage[xs][ys];
			}
			else {
				outImage[i][k] = 255; // ������� ����
			}
		}
	}

	printf("ȸ�� �� Ȯ�� �Ϸ�\n");
	printImage();
}

//void rotateZoom2() { // ȸ��+Ȯ��(�缱�� ����)
//	freeOutputMemory();
//
//	// ȸ�� �� Ȯ�� ���� �� ���� �Է�
//	printf("ȸ�� ���� �Է�\n");
//	int degree = getIntValue();
//	printf("Ȯ�� ���� �Է�\n");
//	int scale = getIntValue();
//	double radian = -degree * 3.141592 / 180.0;
//
//	// ȸ�� �߽� ���
//	int cx = inH / 2; // �̹����� ���̸� �������� �߽� ���
//	int cy = inW / 2; // �̹����� �ʺ� �������� �߽� ���
//
//	// ��� �̹��� ũ�� ���
//	outH = inH * scale;
//	outW = inW * scale;
//
//	// �޸� �Ҵ�
//	mallocOutputMemory();
//
//	double rowRatio = (double)(inH - 1) / (outH - 1);
//	double colRatio = (double)(inW - 1) / (outW - 1);
//
//	// ȸ�� �� Ȯ�� �˰��� ����
//	for (int i = 0; i < outH; i++) {
//		for (int k = 0; k < outW; k++) {
//			// ȸ�� �� Ȯ�뿡 ���� ����� ��ǥ ���
//			double xs = (cos(radian) * (i - cx) - sin(radian) * (k - cy)) / scale + cx;
//			double ys = (sin(radian) * (i - cx) + cos(radian) * (k - cy)) / scale + cy;
//
//			// �缱�� �������� ����Ͽ� �ȼ� �� ���
//			if (xs >= 0 && xs < inW - 1 && ys >= 0 && ys < inH - 1) {
//				int x1 = (int)xs;
//				int y1 = (int)ys;
//				int x2 = x1 + 1;
//				int y2 = y1 + 1;
//
//				double dx = xs - x1;
//				double dy = ys - y1;
//
//				// �缱�� �������� ����Ͽ� ���� �ȼ��� ���� ���
//				double interpolatedValue = (1 - dx) * (1 - dy) * inImage[y1][x1] +
//					dx * (1 - dy) * inImage[y1][x2] +
//					(1 - dx) * dy * inImage[y2][x1] +
//					dx * dy * inImage[y2][x2];
//
//				// ���� ������ ��� �̹����� ���� �ȼ� ����
//				outImage[i][k] = (unsigned char)interpolatedValue;
//			}
//			else {
//				outImage[i][k] = 255; // ������ ����� ��� ������� ����
//			}
//		}
//	}
//	printf("ȸ�� �� Ȯ�� �Ϸ�\n");
//	printImage();
//}

void histoStretch() { // ������׷� ��Ʈ��Ī �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	// ���⼭���� ����ó�� �˰��� ����~~
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
	printf("������׷� ��Ʈ��Ī\n");
	printImage();
}

void endIn() { // ������ Ž�� �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	// ���⼭���� ����ó�� �˰��� ����~~
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
	printf("����-��\n");
	printImage();
}

void histoEqual() { // ������׷� ��Ȱȭ �˰���

	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();


	// 1�ܰ� : �󵵼� ���� (=������׷�)
	int histo[256] = { 0. };
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			histo[inImage[i][k]]++;

	// 2�ܰ� : ���� ������׷� ����
	int sumHisto[256] = { 0, };
	sumHisto[0] = histo[0];
	for (int i = 1; i < 256; i++)
		sumHisto[i] = sumHisto[i - 1] + histo[i];
	// 3�ܰ� : ����ȭ�� ������׷� ���� normalHisto = sumHisto * (1.0 / (inH*inW) * 255.0;
	double normalHisto[256] = { 1.0, };
	for (int i = 0; i < 256; i++) {
		normalHisto[i] = sumHisto[i] * (1.0 / (inH * inW)) * 255.0;
	}
	// 4�ܰ� : inImage�� ����ȭ�� ������ ġȯ
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = (unsigned char)normalHisto[inImage[i][k]];
		}
	}
	printf("������׷� ��Ȱȭ\n");
	printImage();
}

void moveImage() { // �̹��� �̵� �˰���
	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	int a = getIntValue();
	int b = getIntValue();
	int i, k = 0;
	// �Է� �迭 --> ��� �迭
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			// �̹��� �̵� �� ��踦 ����� ��츦 ����Ͽ� �ε����� üũ�Ͽ� ó��
			if (i + a >= 0 && i + a < inH && k - b >= 0 && k - b < inW)
				outImage[i][k] = inImage[i + a][k - b];
			else 
				outImage[i][k] = 0; // ��踦 ����� ��쿡�� 0���� ó���ϰų� �ٸ� ������� ó��
		}
	}
	printf("�̹��� �̵�\n");
	printImage();
}

void mirrorImage() {
	printf("1 = ���� ���� or 2 = �¿� ����\n");
	// �޸� ����
	freeOutputMemory();

	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;

	// �޸� �Ҵ�
	mallocOutputMemory();

	int a = getIntValue();
	// 1�϶��� ����, 2�϶��� �¿� ����

	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (a == 1)
				outImage[i][k] = inImage[inH - 1 - i][k]; // ���� ����
			else
				outImage[i][k] = inImage[i][inW - 1 - k]; // �¿� ����
		}
	}

	// ����ϴ� ���ڿ� ����
	char* mirrorType = (a == 1) ? "���� ��Ī" : "�¿� ��Ī";
	printf("%s\n", mirrorType);

	printImage();
}

void emboss() { // ȭ�ҿ��� ó�� : ������ �˰���

	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = 
	{ {-1.0, 0.0, 0.0}, // ������ ����ũ
	{ 0.0, 0.0, 0.0},
	{ 0.0, 0.0, 1.0} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++) // 2 ��� 1���� ���ص� �Ǳ� ��
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.
			for (int m = 0; m < 3; m++) {
				for (int n = 0; n < 3; n++) {
					S += tmpInImage[i + m][k + n] * mask[m][n];
				}
				tmpOutImage[i][k] = S;
			}
		}
	}
	// ��ó�� (����ũ ���� �հ迡 ����...)
	for (int i = 0; i < outH; i++)
		for (int k = 0; k < outW; k++)
			tmpOutImage[i][k] += 127.0;

	// �ӽ� ��� ����--> ��� ����. 
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
	printf("������\n");
	printImage();
}

void blur() { // ȭ�ҿ��� ó�� : ���� �˰���
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = 
	{ {1. / 9, 1. / 9, 1. / 9}, // ���� ����ũ
	  {1. / 9, 1. / 9, 1. / 9},
	  {1. / 9, 1. / 9, 1. / 9} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// ��ó�� (����ũ ���� �հ迡 ����...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// �ӽ� ��� ����--> ��� ����. 
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
	printf("��\n");
	printImage();
}

void sharp() { // ȭ�ҿ��� ó�� : ������ �˰���
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = 
	{ {0.0, -1.0 ,0.0}, // ������ ����ũ
	  {-1.0 ,5.0, -1.0},
	  {0.0, -1.0, 0.0} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// ��ó�� (����ũ ���� �հ迡 ����...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// �ӽ� ��� ����--> ��� ����. 
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
	printf("������\n");
	printImage();
}

void edge1() { // ��輱����: ���� ���� ���� ����ũ
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�
	mallocOutputMemory();

	////////////////////
	// ȭ�� ���� ó��
	////////////////////

	double mask[3][3] = { {0.0, 0.0, 0.0}, // ���� ���� ���� ����ũ
						  {-1.0, 1.0, 0.0},
						  {0.0,  0.0, 0.0} };

	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ� 
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// ** ȸ�� ���� ** 
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3)�� ������ �߽����� �� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// ��ó�� (����ũ ���� �հ迡 ����...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// �ӽ� ��� ���� --> ��� ����. 
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
	printf("��輱 ó��\n");
	printImage();
}

