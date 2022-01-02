#pragma once

#include <Eigen/Eigen>


namespace GLMatrix {
	static Eigen::Matrix4f lookAt(Eigen::Vector3f eye, Eigen::Vector3f center, Eigen::Vector3f up) {
		if (eye[0] == center[0] && eye[1] == center[1] && eye[2] == center[2]) {
			return Eigen::Matrix4f::Identity();
		}

		float z0, z1, z2, x0, x1, x2, y0, y1, y2, len;
		z0 = eye[0] - center[0];
		z1 = eye[1] - center[1];
		z2 = eye[2] - center[2];

		len = 1 / sqrtf(z0*z0 + z1 * z1 + z2 * z2);
		z0 *= len;
		z1 *= len;
		z2 *= len;

		x0 = up[1] * z2 - up[2] * z1;
		x1 = up[2] * z0 - up[0] * z2;
		x2 = up[0] * z1 - up[1] * z0;

		len = sqrtf(x0*x0 + x1 * x1 + x2 * x2);
		if (fabsf(len) < 0.000001f) {
			x0 = 0;
			x1 = 0;
			x2 = 0;
		}
		else {
			len = 1 / len;
			x0 *= len;
			x1 *= len;
			x2 *= len;
		}

		y0 = z1 * x2 - z2 * x1;
		y1 = z2 * x0 - z0 * x2;
		y2 = z0 * x1 - z1 * x0;

		len = sqrtf(y0*y0 + y1 * y1 + y2 * y2);
		if (fabsf(len) < 0.000001f) {
			y0 = 0;
			y1 = 0;
			y2 = 0;
		}
		else {
			len = 1 / len;
			y0 *= len;
			y1 *= len;
			y2 *= len;
		}

		Eigen::Matrix4f result;
		result <<
			x0, x1, x2, -(x0*eye[0] + x1 * eye[1] + x2 * eye[2]),
			y0, y1, y2, -(y0*eye[0] + y1 * eye[1] + y2 * eye[2]),
			z0, z1, z2, -(z0*eye[0] + z1 * eye[1] + z2 * eye[2]),
			0, 0, 0, 1;

		return result;
	}

	static Eigen::Matrix4f ortho(const float bottom, const float top, const float left, const float right, const float _near, const float _far)
	{
		Eigen::Matrix4f result;
		result(0, 0) = 2 / (right - left);
		result(1, 0) = 0;
		result(2, 0) = 0;
		result(3, 0) = 0;
		result(0, 1) = 0;
		result(1, 1) = 2 / (top - bottom);
		result(2, 1) = 0;
		result(3, 1) = 0;
		result(0, 2) = 0;
		result(1, 2) = 0;
		result(2, 2) = -2 / (_far - _near);
		result(3, 2) = 0;
		result(0, 3) = -(right + left) / (right - left);
		result(1, 3) = -(top + bottom) / (top - bottom);
		result(2, 3) = -(_far + _near) / (_far - _near);
		result(3, 3) = 1;

		return result;
	}

	static Eigen::Matrix4f frustum(float left, float right, float bottom, float top, float nearDistance, float farDistance) {
		float rl = right - left;
		float tb = top - bottom;
		float fn = farDistance - nearDistance;
		Eigen::Matrix4f result;
		result <<
			(nearDistance * 2 / rl), 0, (right + left) / rl, 0,
			0, (nearDistance * 2 / tb), (top + bottom) / tb, 0,
			0, 0, -(farDistance + nearDistance) / fn, -(farDistance * nearDistance * 2) / fn,
			0, 0, -1, 0;

		return result;
	}

	static Eigen::Matrix4f perspective(float fovy, float aspect, float nearDistance, float farDistance) {
		float top = nearDistance * tanf(fovy * 3.14159265359 / 360.0f);
		float right = top * aspect;
		return frustum(-right, right, -top, top, nearDistance, farDistance);
	}

	static Eigen::Matrix4f cameraProjectionMatrix(float width, float height, float fx, float fy, float px, float py, float nearDistance, float farDistance) {

		float fn = farDistance - nearDistance;
		Eigen::Matrix4f result;

		result <<
			2.0f * fx / width, 0, -(2.0f * px - width) / width, 0,
			0, 2.0f * fy / height, -(2.0f*((height - 1.0f) - py) - height) / height, 0,
			0, 0, -(farDistance + nearDistance) / fn, -(2.0f * farDistance * nearDistance) / fn,
			0, 0, -1.0f, 0;

		return result;
	}

	static Eigen::Matrix4f convetCameraAxisToGLAxis() {
		Eigen::Matrix4f result;

		result <<
			1, 0, 0, 0,
			0, -1, 0, 0,
			0, 0, -1, 0,
			0, 0, 0, 1;

		return result;
	}

	static Eigen::Matrix4f rotate(const Eigen::Vector3f& axis, const float radianAngle)
	{
		Eigen::Matrix4f result;
		result.setIdentity();

		if (axis.x() == 0 && axis.y() == 0 && axis.z() == 0)
			return result;

		float r = radianAngle;
		float s = sin(r);
		float c = cos(r);
		float omc = 1.0f - cos(r);

		float x = axis.x();
		float y = axis.y();
		float z = axis.z();

		result(0, 0) = c + x * x * omc;
		result(1, 0) = x * y * omc - z * s;
		result(2, 0) = z * x * omc + y * s;
		result(0, 1) = x * y * omc + z * s;
		result(1, 1) = c + y * y * omc;
		result(2, 1) = z * y * omc - x * s;
		result(0, 2) = x * z * omc - y * s;
		result(1, 2) = y * z * omc + x * s;
		result(2, 2) = c + z * z * omc;

		return result;
	}
};
