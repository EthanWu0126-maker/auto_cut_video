#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <iomanip>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "用法: " << argv[0] << " <audio_path>\n";
		return 1;
	}

	std::string infile = argv[1];

	const int sample_rate = 44100;
	const int channels = 1;
	const double window_seconds = 0.5;

	std::string cmd = "ffmpeg -i \"" + infile + "\" -f f32le -acodec pcm_f32le -ac " + std::to_string(channels) +
					  " -ar " + std::to_string(sample_rate) + " -hide_banner -loglevel error -vn -";

	FILE* pipe = POPEN(cmd.c_str(), "rb");
	if (!pipe) {
		std::cerr << "请将ffmpeg添加到系统PATH中或放置于该文件目录下\n";
		return 2;
	}

	const size_t samples_per_chunk = static_cast<size_t>(static_cast<double>(sample_rate) * channels * window_seconds);
	if (samples_per_chunk == 0) {
		std::cerr << "缓冲区大小为 0，请检查采样率和窗口大小设置。\n";
		PCLOSE(pipe);
		return 3;
	}
	std::vector<float> buffer(samples_per_chunk);

	std::ofstream fout("mark_temp.txt", std::ios::out | std::ios::trunc);
	if (!fout) {
		std::cerr << "无法创建文件 mark_temp.txt\n";
		PCLOSE(pipe);
		return 4;
	}
	fout.setf(std::ios::fixed);
	fout << std::setprecision(6);

	bool first = true;

	while (true) {
		size_t read = fread(buffer.data(), sizeof(float), samples_per_chunk, pipe);
		if (read == 0) break;

		double sum_sqr = 0.0;
		for (size_t i = 0; i < read; ++i) {
			double v = buffer[i];
			sum_sqr += v * v;
		}

		double rms = 0.0;
		if (read > 0) rms = std::sqrt(sum_sqr / static_cast<double>(read));

		const double eps = 1e-12;
		double db = -200.0;
		if (rms > eps) db = 20.0 * std::log10(rms);

		if (!first) {
			fout << ",";
		}
		fout << db;
		first = false;

		if (read < samples_per_chunk) break;
	}

	PCLOSE(pipe);
	fout << std::endl;
	fout.close();
	std::cout << "保存到零时文件成功\n";
	return 0;
}
