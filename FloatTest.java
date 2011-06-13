
public class FloatTest {
	public static void main(String[] args) {
		try {
			System.out.write( f2b(1f) );
			System.out.write( f2b(2f) );
			System.out.write( f2b(3f) );
		
			System.out.write( f2b(3.2f) );
			System.out.write( f2b(2.3f) );
			System.out.write( f2b(-5.6f) );
			}
		catch(Exception e) {}
		}
	
	public static byte[] f2b(float f) {
		int MASK = 0xff;
		int param = Float.floatToRawIntBits(f);
		byte[] result = new byte[4];
		for (int i = 0; i < 4; i++) {
			int offset = (result.length - 1 - i) * 8;
			result[i] = (byte) ((param >>> offset) & MASK);
		}
		return result;
		}
	}
