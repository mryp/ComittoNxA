package src.comitton.stream;

import java.io.IOException;
import java.io.InputStream;

import src.comitton.common.DEF;

import android.os.Handler;
import android.os.Message;
import static src.comitton.stream.ImageManager.MSG_LOADING;

public class RarInputStream extends InputStream {
	public static final int OFFSET_RAR_HCRC = 0;
	public static final int OFFSET_RAR_HTYPE = 2;
	public static final int OFFSET_RAR_HFLAGS = 3;
	public static final int OFFSET_RAR_HSIZE = 5;
	public static final int OFFSET_RAR_ASIZE = 7;

	// Marker Block
	// Archive Header
	public static final int OFFSET_RAR_RESV1 = 7; // 2bytes
	public static final int OFFSET_RAR_RESV2 = 9; // 4bytes

	// FileHeader
	public static final int OFFSET_RAR_PKSIZE = 7; // 4bytes
	public static final int OFFSET_RAR_UNSIZE = 11; // 4bytes
	public static final int OFFSET_RAR_HOSTOS = 15; // 1byte
	public static final int OFFSET_RAR_FCRC = 16; // 4bytes
	public static final int OFFSET_RAR_FTIME = 20; // 4bytes
	public static final int OFFSET_RAR_UNPVER = 24; // 1byte
	public static final int OFFSET_RAR_METHOD = 25; // 1byte
	public static final int OFFSET_RAR_FNSIZE = 26; // 2bytes
	public static final int OFFSET_RAR_ATTRIB = 28; // 4bytes
	public static final int OFFSET_RAR_FNAME = 32; // OFFSET_RAR_FNSIZE
//
	public static final int RAR_HTYPE_MARK = 0x72;
	public static final int RAR_HTYPE_MAIN = 0x73;
	public static final int RAR_HTYPE_FILE = 0x74;

	public static final byte RAR_METHOD_STORING = 0x30;

	Handler mHandler;
	InputStream	mInputStream;
	int mPage;

	// 4バイト数値取得
	public int getInt(byte b[], int pos) {
		int val;
		val = ((int) b[pos] & 0x000000FF) | (((int) b[pos + 1] << 8) & 0x0000FF00) | (((int) b[pos + 2] << 16) & 0x00FF0000) | (((int) b[pos + 3] << 24) & 0xFF000000);

		return val;
	}

	// 2バイト数値取得
	public short getShort(byte b[], int pos) {
		int val;
		val = ((int) b[pos] & 0x000000FF) | (((int) b[pos + 1] << 8) & 0x0000FF00);

		return (short) val;
	}

	public RarInputStream(InputStream is, int page, FileListItem fi) throws IOException {
		this(is, page, fi, null);
	}

	public RarInputStream(InputStream is, int page, FileListItem fi, Handler handler) throws IOException {
		mInputStream = is;
		mHandler = handler;
		mPage = page;
		int cmplen = fi.cmplen;
		int orglen = fi.orglen;
		int header = fi.header;
		byte rarver = fi.version;
		boolean nocomp = fi.nocomp;

		byte buff[] = new byte[1024 * 100];
		int	retsize;
		int readsize;
		int datasize = cmplen - header;
		int ret;

		ret = CallJniLibrary.rarInit(cmplen - header, orglen, rarver, nocomp);
		if (ret != 0) {
			throw new IOException("Illegal function call.");
		}

		retsize = is.read(buff, 0, header);
		int remain = datasize;

		int msgCount = 0;
		long startTime = System.currentTimeMillis();
		while (remain > 0) {
			readsize = buff.length;
			if (readsize > remain) {
				readsize = remain;
			}
			retsize = is.read(buff, 0, readsize);
			if (retsize <= 0) {
				break;
			}
			remain -= retsize;
			CallJniLibrary.rarWrite(buff, 0, retsize);
			if(mHandler != null) {
				long nowTime = System.currentTimeMillis();
				if (nowTime - startTime > (msgCount + 1) * 200) {
					msgCount++;
					int prog = (int) ((long) (datasize - remain) * 100 / datasize);
					int rate = (int) ((long) (datasize - remain) * 10 / (nowTime - startTime));
					Message message = new Message();
					message.what = MSG_LOADING;
					message.arg1 = prog << 24;
					message.arg2 = rate;
					message.obj = null;
					mHandler.sendMessage(message);
				}
			}
		}
		// 変換
		CallJniLibrary.rarDecomp();
	}

	// 読み込み済みのページを返す
	public int getLoadPage() {
		return mPage;
	}

	// 現在の展開ファイルをもう一度最初から返す
	public void initSeek() {
		CallJniLibrary.rarInitSeek();
	}

	@Override
	public int read() throws IOException {
		// TODO 自動生成されたメソッド・スタブ
		return 0;
	}

	@Override
	public int read(byte buf[], int off, int len) throws IOException {
		int ret = CallJniLibrary.rarRead(buf, off, len);
		if (ret == 0) {
			ret = -1;
		}
		return ret;
	}

	public void close() {
		CallJniLibrary.rarClose();
	}
}
