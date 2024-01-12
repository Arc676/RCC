#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

#include <memory>
#include <utility>

#include "Stream/buffer.h"
#include "interface.h"
#include "libcamera/camera.h"

class CameraData {
	using SharedCamera = std::shared_ptr<libcamera::Camera>;
	SharedCamera camera;

public:
	explicit CameraData(SharedCamera camera)
		: camera(std::move(camera)) {}

	[[nodiscard]] bool exists() const { return camera != nullptr; }

	void serialize(Buffer<byte>&) const;
};

class CameraProperties {
	using Map = std::unordered_map<unsigned int, std::string>;

	using iterator       = Map::iterator;
	using const_iterator = Map::const_iterator;

	Map props;

public:
	void deserialize(Buffer<const byte>&);

	iterator begin();
	iterator end();

	const_iterator cbegin() const;
	const_iterator cend() const;
};

#endif
