/*
 * Copyright (C) 2008  Barracuda Networks, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 *
 */

#ifndef PSIMEDIAPROVIDER_H
#define PSIMEDIAPROVIDER_H

#include <QString>
#include <QList>
#include <QByteArray>
#include <QSize>
#include <QObject>

class QImage;
class QIODevice;

// since we cannot put signals/slots in Qt "interfaces", we use the following
//   defines to hint about signals/slots that derived classes should provide
#define HINT_SIGNALS protected
#define HINT_PUBLIC_SLOTS public
#define HINT_METHOD(x)

namespace PsiMedia {

class ProducerContext;
class ReceiverContext;

class Provider
{
public:
	virtual ~Provider() {}

	virtual bool init(const QString &resourcePath) = 0;
	virtual QString creditName() = 0;
	virtual QString creditText() = 0;

	virtual void initEngine() = 0;
	virtual ProducerContext *createProducer() = 0;
	virtual ReceiverContext *createReceiver() = 0;

HINT_SIGNALS:
	HINT_METHOD(initEngineFinished())
};

class PDevice
{
public:
	enum Type
	{
		AudioIn,
		AudioOut,
		VideoIn
	};

	Type type;
	QString name;
	QString id;
};

class PAudioParams
{
public:
	QString codec;
	int sampleRate;
	int sampleSize;
	int channels;
};

class PVideoParams
{
public:
	QString codec;
	QSize size;
	int fps;
};

class PPayloadInfo
{
public:
	class Parameter
	{
	public:
		QString name;
		QString value;
	};

	int id;
	QString name;
	int clockrate;
	int channels;
	int ptime;
	int maxptime;
	QList<Parameter> parameters;
};

class PRtpPacket
{
public:
	QByteArray rawValue;
	int portOffset;
};

class RtpChannelContext
{
public:
	virtual ~RtpChannelContext() {}

	virtual void setEnabled(bool b) = 0;

	virtual int packetsAvailable() const = 0;
	virtual PRtpPacket read() = 0;
	virtual void write(const PRtpPacket &rtp) = 0;

HINT_SIGNALS:
	HINT_METHOD(readyRead())
	HINT_METHOD(packetsWritten(int count))
};

#ifdef QT_GUI_LIB
class VideoWidgetContext
{
public:
	virtual ~VideoWidgetContext() {}

	virtual QSize desired_size() const = 0;
	virtual void show_frame(const QImage &img) = 0;
};
#endif

class ProducerContext
{
public:
	enum Error
	{
		ErrorGeneric
	};

	virtual ~ProducerContext() {}

	virtual void setAudioInputDevice(const QString &deviceId) = 0;
	virtual void setVideoInputDevice(const QString &deviceId) = 0;
	virtual void setFileInput(const QString &fileName) = 0;
	virtual void setFileDataInput(const QByteArray &fileData) = 0;
#ifdef QT_GUI_LIB
	virtual void setVideoWidget(VideoWidgetContext *widget) = 0;
#endif

	virtual void setAudioPayloadInfo(const QList<PPayloadInfo> &info) = 0;
	virtual void setVideoPayloadInfo(const QList<PPayloadInfo> &info) = 0;
	virtual void setAudioParams(const QList<PAudioParams> &params) = 0;
	virtual void setVideoParams(const QList<PVideoParams> &params) = 0;

	virtual void start() = 0;
	virtual void transmitAudio(int paramsIndex) = 0;
	virtual void transmitVideo(int paramsIndex) = 0;
	virtual void pauseAudio() = 0;
	virtual void pauseVideo() = 0;
	virtual void stop() = 0;

	virtual QList<PPayloadInfo> audioPayloadInfo() const = 0;
	virtual QList<PPayloadInfo> videoPayloadInfo() const = 0;
	virtual QList<PAudioParams> audioParams() const = 0;
	virtual QList<PVideoParams> videoParams() const = 0;

	virtual int volume() const = 0; // 0 (mute) to 100
	virtual void setVolume(int level) = 0;

	virtual Error errorCode() const = 0;

	virtual RtpChannelContext *audioRtpChannel() = 0;
	virtual RtpChannelContext *videoRtpChannel() = 0;

HINT_SIGNALS:
	HINT_METHOD(started())
	HINT_METHOD(stopped())
	HINT_METHOD(finished()) // for file playback only
	HINT_METHOD(error())
};

class ReceiverContext
{
public:
	enum Error
	{
		ErrorGeneric,
		ErrorSystem,
		ErrorCodec
	};

	virtual ~ReceiverContext() {}

	virtual void setAudioOutputDevice(const QString &deviceId) = 0;
#ifdef QT_GUI_LIB
	virtual void setVideoWidget(VideoWidgetContext *widget) = 0;
#endif
	virtual void setRecorder(QIODevice *recordDevice) = 0;

	virtual void setAudioPayloadInfo(const QList<PPayloadInfo> &info) = 0;
	virtual void setVideoPayloadInfo(const QList<PPayloadInfo> &info) = 0;
	virtual void setAudioParams(const QList<PAudioParams> &params) = 0;
	virtual void setVideoParams(const QList<PVideoParams> &params) = 0;

	virtual void start() = 0;
	virtual void stop() = 0;

	virtual QList<PPayloadInfo> audioPayloadInfo() const = 0;
	virtual QList<PPayloadInfo> videoPayloadInfo() const = 0;
	virtual QList<PAudioParams> audioParams() const = 0;
	virtual QList<PVideoParams> videoParams() const = 0;

	virtual int volume() const = 0; // 0 (mute) to 100
	virtual void setVolume(int level) = 0;

	virtual Error errorCode() const = 0;

	virtual RtpChannelContext *audioRtpChannel() = 0;
	virtual RtpChannelContext *videoRtpChannel() = 0;

HINT_SIGNALS:
	HINT_METHOD(started())
	HINT_METHOD(stopped())
	HINT_METHOD(error())
};

}

Q_DECLARE_INTERFACE(PsiMedia::Provider, "org.psi-im.psimedia.Provider/1.0")
Q_DECLARE_INTERFACE(PsiMedia::RtpChannelContext, "org.psi-im.psimedia.RtpChannelContext/1.0")
Q_DECLARE_INTERFACE(PsiMedia::ProducerContext, "org.psi-im.psimedia.ProducerContext/1.0")
Q_DECLARE_INTERFACE(PsiMedia::ReceiverContext, "org.psi-im.psimedia.ReceiverContext/1.0")

#endif
