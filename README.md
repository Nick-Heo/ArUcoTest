# ArUco Test with OpenGL

이 저장소는 OpenCV 의 ArUco 마커 인식 기능 테스트 및 OpenGL 을 통한 Viewer 를 구현한 저장소 입니다.

실시간 카메라로 작동하며, 작동 환경은 다음과 같습니다.

- Visual Studio 2017(vc15)
- Eigen 3.3.7
- OpenGL / glew / freeglut

다중 마커 인식을 지원하며, 기본적으로는 6x6 ArUco marker을 지원합니다. 

영상 정보의 경우 고의적으로 intrinsic parameter을 무시하였으며, 사용할 경우엔 intrinsic 정보를 입력하여주면 정상 동작 합니다.

렌즈 왜곡의 경우 마찬가지로 고의적으로 무시하였으며, 일반적으로 렌즈 왜곡이 적은 웹캠, 스마트폰을 대상으로 합니다.

OpenGL 의 Rendering 방식은 좌 상단의 마커 영상뷰는 Render to texture 방식으로 FrameBuffer을 이용하여 구현하였으며, 우측의 3D 뷰는 각각의 마커 이미지를 좌표축과 함께 표시하였습니다.

각 class의 간단한 설명은 다음과 같습니다

---

## class ArUcoController

```cpp
//ArUco marker 포즈 계산시 필요한 camera intrinsic.
// width : image width
// height : image height
// fx : focal length x
// fy : focal length y
// px : image center x
// py : image center y
void setCameraIntrinsic(double width, double height, double fx, double fy, double px, double py);

//인식하는 ArUco marker pattern을 설정.
// pattern : opencv 의 aruco pattern
void setArUcoPattern(cv::aruco::PREDEFINED_DICTIONARY_NAME pattern);

//각 ArUco marker의 preset 크기. 기본 마커 사이즈를 1 이라 놓고 상대 크기로 계산하여 입력.
// id : 마커의 id
// size : 마커의 크기
void presetMarkerSize(int id, double size);

//이미지의 마커 인식 및 포즈 계산 함수
// image : 입력 받는 이미지
// outMarkers : 인식한 마커의 정보 출력
// return : 인식한 마커의 갯수
int calculateArUcoPose(cv::Mat& image, std::vector<Marker>& outMarkers);
```

## struct Marker

```cpp
//마커의 아이디
int id;

//마커의 크기
double size;

//마커의 자세정보
Eigen::Matrix4d pose;

//마커의 인식시 사용된 pattern
cv::aruco::PREDEFINED_DICTIONARY_NAME pattern;

//마커의 id로 부터 합성된 마커 이미지
cv::Mat image;
```

## class Viewer

```cpp
//Viewer 의 초기화
// argc : main 함수의 argc
// argv : main 함수의 argv
// shaderFolderPath : shader 파일들이 입는 상위 경로
void initialize(int argc, char** argv, const string& shaderFolderPath);

//카메라 이미지 입력
// inputImage : 입력 받는 이미지
void setCameraImage(cv::Mat& inputImage);

//인식한 마커의 정보 입력
// markers : 마커 정보
void setMarker(std::vector<Marker>& markers);
```
