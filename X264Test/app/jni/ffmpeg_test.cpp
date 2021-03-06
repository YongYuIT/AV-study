#include"com_hsdi_x264test_FFmpegTest.h"
#include<iostream>
#include<android\log.h>
using namespace std;

JNIEXPORT jboolean JNICALL Java_com_hsdi_x264test_FFmpegTest_ffmpeg_1test_1AV_1package
(JNIEnv *env, jclass j_class, jstring input_path, jstring output_path){
	AVOutputFormat *ofmt = NULL;
	//Input AVFormatContext and Output AVFormatContext  
	AVFormatContext *ifmt_ctx_v = NULL, *ifmt_ctx_a = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	int ret, i;
	int videoindex_v = -1, videoindex_out = -1;
	int audioindex_a = -1, audioindex_out = -1;
	int frame_index = 0;
	int64_t cur_pts_v = 0, cur_pts_a = 0;

	const char *in_filename_v = env->GetStringUTFChars(input_path, 0);
	const char *in_filename_a = "/sdcard/test_a.mp3";

	const char *out_filename = env->GetStringUTFChars(output_path, 0);//Output file URL  
	av_register_all();
	//Input  
	if ((ret = avformat_open_input(&ifmt_ctx_v, in_filename_v, 0, 0)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "Could not open input file.");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx_v, 0)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "Failed to retrieve input stream information");
		goto end;
	}

	if ((ret = avformat_open_input(&ifmt_ctx_a, in_filename_a, 0, 0)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "Could not open input file.");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx_a, 0)) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "Failed to retrieve input stream information");
		goto end;
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "===========Input Information==========\n");
	av_dump_format(ifmt_ctx_v, 0, in_filename_v, 0);
	av_dump_format(ifmt_ctx_a, 0, in_filename_a, 0);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "======================================\n");
	//Output  
	avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
	if (!ofmt_ctx) {
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;

	for (i = 0; i < ifmt_ctx_v->nb_streams; i++) {
		//Create output AVStream according to input AVStream  
		if (ifmt_ctx_v->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			AVStream *in_stream = ifmt_ctx_v->streams[i];
			AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
			videoindex_v = i;
			if (!out_stream) {
				__android_log_print(ANDROID_LOG_INFO, "yuyong", "Failed allocating output stream\n");
				ret = AVERROR_UNKNOWN;
				goto end;
			}
			videoindex_out = out_stream->index;
			//Copy the settings of AVCodecContext  
			if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
				__android_log_print(ANDROID_LOG_INFO, "yuyong", "Failed to copy context from input to output stream codec context\n");
				goto end;
			}
			out_stream->codec->codec_tag = 0;
			if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
				out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
			break;
		}
	}

	for (i = 0; i < ifmt_ctx_a->nb_streams; i++) {
		//Create output AVStream according to input AVStream  
		if (ifmt_ctx_a->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
			AVStream *in_stream = ifmt_ctx_a->streams[i];
			AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
			audioindex_a = i;
			if (!out_stream) {
				__android_log_print(ANDROID_LOG_INFO, "yuyong", "Failed allocating output stream\n");
				ret = AVERROR_UNKNOWN;
				goto end;
			}
			audioindex_out = out_stream->index;
			//Copy the settings of AVCodecContext  
			if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0) {
				__android_log_print(ANDROID_LOG_INFO, "yuyong", "Failed to copy context from input to output stream codec context\n");
				goto end;
			}
			out_stream->codec->codec_tag = 0;
			if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
				out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

			break;
		}
	}

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "==========Output Information==========\n");
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "======================================\n");
	//Open output file  
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0) {
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "Could not open output file '%s'", out_filename);
			goto end;
		}
	}
	//Write file header  
	if (avformat_write_header(ofmt_ctx, NULL) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "Error occurred when opening output file\n");
		goto end;
	}


	//FIX  
#if USE_H264BSF  
	AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");   
#endif  
#if USE_AACBSF  
	AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");   
#endif  

	while (1) {
		AVFormatContext *ifmt_ctx;
		int stream_index = 0;
		AVStream *in_stream, *out_stream;

		//Get an AVPacket  
		if (av_compare_ts(cur_pts_v, ifmt_ctx_v->streams[videoindex_v]->time_base, cur_pts_a, ifmt_ctx_a->streams[audioindex_a]->time_base) <= 0){
			ifmt_ctx = ifmt_ctx_v;
			stream_index = videoindex_out;

			if (av_read_frame(ifmt_ctx, &pkt) >= 0){
				do{
					in_stream = ifmt_ctx->streams[pkt.stream_index];
					out_stream = ofmt_ctx->streams[stream_index];

					if (pkt.stream_index == videoindex_v){
						//FIX：No PTS (Example: Raw H.264)  
						//Simple Write PTS  
						if (pkt.pts == AV_NOPTS_VALUE){
							//Write PTS  
							AVRational time_base1 = in_stream->time_base;
							//Duration between 2 frames (us)  
							int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
							//Parameters  
							pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
							pkt.dts = pkt.pts;
							pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
							frame_index++;
						}

						cur_pts_v = pkt.pts;
						break;
					}
				} while (av_read_frame(ifmt_ctx, &pkt) >= 0);
			}
			else{
				break;
			}
		}
		else{
			ifmt_ctx = ifmt_ctx_a;
			stream_index = audioindex_out;
			if (av_read_frame(ifmt_ctx, &pkt) >= 0){
				do{
					in_stream = ifmt_ctx->streams[pkt.stream_index];
					out_stream = ofmt_ctx->streams[stream_index];

					if (pkt.stream_index == audioindex_a){

						//FIX：No PTS  
						//Simple Write PTS  
						if (pkt.pts == AV_NOPTS_VALUE){
							//Write PTS  
							AVRational time_base1 = in_stream->time_base;
							//Duration between 2 frames (us)  
							int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
							//Parameters  
							pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
							pkt.dts = pkt.pts;
							pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
							frame_index++;
						}
						cur_pts_a = pkt.pts;

						break;
					}
				} while (av_read_frame(ifmt_ctx, &pkt) >= 0);
			}
			else{
				break;
			}

		}

		//FIX:Bitstream Filter  
#if USE_H264BSF  
		av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
#endif  
#if USE_AACBSF  
		av_bitstream_filter_filter(aacbsfc, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);  
#endif  


		//Convert PTS/DTS  
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;
		pkt.stream_index = stream_index;

		__android_log_print(ANDROID_LOG_INFO, "yuyong", "Write 1 Packet. size:%5d\tpts:%lld\n", pkt.size, pkt.pts);
		//Write  
		if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0) {
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "Error muxing packet\n");
			break;
		}
		av_free_packet(&pkt);

	}
	//Write file trailer  
	av_write_trailer(ofmt_ctx);

#if USE_H264BSF  
	av_bitstream_filter_close(h264bsfc);
#endif  
#if USE_AACBSF  
	av_bitstream_filter_close(aacbsfc);  
#endif  

end:
	avformat_close_input(&ifmt_ctx_v);
	avformat_close_input(&ifmt_ctx_a);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "Error occurred.\n");
		return -1;
	}
	return 0;
}

JNIEXPORT jboolean JNICALL Java_com_hsdi_x264test_FFmpegTest_ffmpeg_1test_1V_1package
(JNIEnv *env, jclass j_class, jstring input_path, jstring output_path)
{
	const char *input_file_path = env->GetStringUTFChars(input_path, 0);
	const char *output_file_path = env->GetStringUTFChars(output_path, 0);

	//注册FFmpeg所有组件
	av_register_all();

	//打开文件流
	AVFormatContext *input_ctx = NULL;
	if (avformat_open_input(&input_ctx, input_file_path, 0, 0) < 0){
		//打开输入文件失败
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "open input file failed");
		return false;
	}

	//获取媒体文件信息，检测是否被支持
	//avformat_find_stream_info：读取音视频流描述信息，作用是检测流数据是否被支持，正常执行返回0。
	if (avformat_find_stream_info(input_ctx, 0) < 0){
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "do not support type");
		return false;
	}

	//这是一个调试函数，用于打印关于音视频流数据基本信息的日志
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "------------------start");
	av_dump_format(input_ctx, 0, input_file_path, 0);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "------------------end");

	//初始化输出Context
	AVFormatContext *output_ctx = NULL;
	avformat_alloc_output_context2(&output_ctx, NULL, NULL, output_file_path);
	if (output_ctx == NULL){
		//打开输出文件失败
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "open output file failed");
		return false;
	}
	AVOutputFormat *output_fmt = NULL;
	output_fmt = output_ctx->oformat;


	int total_out_index = -1;
	for (int i = 0; i < input_ctx->nb_streams; i++){
		AVStream *instream = input_ctx->streams[i];
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "get stream %i --> %i", i, instream->codec->codec_type);
		//如果输入流类型不是视频类型，跳过
		if (instream->codec->codec_type != AVMEDIA_TYPE_VIDEO)
		{
			continue;
		}
		AVStream *outstream = avformat_new_stream(output_ctx, instream->codec->codec);
		if (outstream == NULL)
		{
			//创建流失败
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "create stream failed");
			return false;
		}
		total_out_index = outstream->index;
		if (avcodec_copy_context(outstream->codec, instream->codec) < 0){
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "codec info copy failed");
			return false;
		}

		outstream->codec->codec_tag = 0;
		if (output_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			outstream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		break;
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "total_out_index is %i", total_out_index);

	__android_log_print(ANDROID_LOG_INFO, "yuyong", "------------------start");
	av_dump_format(output_ctx, 0, output_file_path, 1);
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "------------------end");

	if (!(output_fmt->flags & AVFMT_NOFILE)) {
		if (avio_open(&output_ctx->pb, output_file_path, AVIO_FLAG_WRITE) < 0) {
			//打开输出文件流失败
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "open output file failed");
			return false;
		}
	}

	//写文件头失败
	if (avformat_write_header(output_ctx, NULL) < 0) {
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "write head data failed");
		return false;
	}

	//转换器配置
	//h264有两种封装模式，一种是annexb模式，这种模式就是x264编码器默认输出的格式（一般文件以.h264结尾）。这种模式也叫传统模式。
	//                                      包含startcode信息，SPS、PPS信息在ES中
	//                    另一种是mp4模式（一般以.mp4或者.mvk结尾）。没有startcode信息，SPS、PPS信息在Container中，每帧数据前面是这帧的长度。
	//这两种模式可以用av_bitstream_filter_filter来转换。
	AVBitStreamFilterContext* h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");

	AVPacket pkt;
	int frame_index = 0;
	int64_t cur_pts_v = 0;
	while (true) {
		AVStream *in_stream;
		AVStream *out_stream;
		//生成打包数据
		if (av_read_frame(input_ctx, &pkt) >= 0){
			do{
				in_stream = input_ctx->streams[pkt.stream_index];
				out_stream = output_ctx->streams[total_out_index];
				if (pkt.stream_index == total_out_index){
					if (pkt.pts == AV_NOPTS_VALUE){
						AVRational time_base1 = in_stream->time_base;
						int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(in_stream->r_frame_rate);
						pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
						pkt.dts = pkt.pts;
						pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
						frame_index++;
					}
					cur_pts_v = pkt.pts;
					break;
				}
			} while (av_read_frame(input_ctx, &pkt) >= 0);
		}
		else{
			break;
		}
		//格式转换
		av_bitstream_filter_filter(h264bsfc, in_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);
		//添加附件信息
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;
		pkt.stream_index = total_out_index;

		//存盘
		__android_log_print(ANDROID_LOG_INFO, "yuyong", "write a pkt --> size:%5d --> pts:%lld", pkt.size, pkt.pts);
		if (av_interleaved_write_frame(output_ctx, &pkt) < 0) {
			__android_log_print(ANDROID_LOG_INFO, "yuyong", "pack failed");
			break;
		}
		av_free_packet(&pkt);
	}
	__android_log_print(ANDROID_LOG_INFO, "yuyong", "pack success");

	//写文件尾
	av_write_trailer(output_ctx);

	//释放资源
	av_bitstream_filter_close(h264bsfc);
	avformat_close_input(&input_ctx);
	if (output_ctx && !(output_ctx->flags & AVFMT_NOFILE))
		avio_close(output_ctx->pb);
	avformat_free_context(output_ctx);

	//释放JNI资源
	env->ReleaseStringUTFChars(input_path, input_file_path);
	env->ReleaseStringUTFChars(output_path, output_file_path);

	return true;
}