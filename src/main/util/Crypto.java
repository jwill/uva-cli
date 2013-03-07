package util;

import j.io.*;

import java.util.*;
import java.io.*;

import java.security.SecureRandom;
import java.security.GeneralSecurityException;

import javax.crypto.*;
import javax.crypto.spec.*;

public final class Crypto
{
    private final byte[] raw;

    public Crypto()
        throws IOException, GeneralSecurityException
    {
        this.raw = genRandom(readKey());
    }

    @Override
    protected void finalize()
    {
        // zero out the key
        for (int i = 0; i < this.raw.length; i++)
            this.raw[i] = 0;
    }

    private static byte[] readKey()
        throws IOException
    {
        String homePath = System.getProperty("user.home");
        String privKeyFilePath = PathUtil.combine(homePath, ".ssh", "id_rsa");
        InputStream in = null;
        StringBuilder sb = new StringBuilder();

        // read key file
        try
        {
            in = new FileInputStream(privKeyFilePath);
            BufferedReader br = new BufferedReader(
                    new InputStreamReader(in, "UTF-8"));

            String line = null;
            while ((line = br.readLine()) != null)
            {
                line = line.trim();
                if (! line.startsWith("----")) 
                    sb.append(line);        
            }
        }
        finally
        {
            if (in != null) in.close();
        }

        String keyStr = sb.toString();

        // zero out the key
        int keyStrLen = sb.length();
        for (int i = 0; i < keyStrLen; i++)
        {
            sb.setCharAt(i, '\0');
        }

        return keyStr.getBytes("UTF-8");
    }

    /**
     * @return an array of byte array. byte[0] is the array of encrypted
     *         bytes while byte[1] is the IV.
     */
    public byte[][] encrypt(byte[] data) 
        throws GeneralSecurityException 
    {
        SecretKeySpec skeySpec = new SecretKeySpec(this.raw, "AES");
        byte[] iv = genRandom(16);
        IvParameterSpec ivspec = new IvParameterSpec(iv);

        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        cipher.init(Cipher.ENCRYPT_MODE, skeySpec, ivspec);
        byte[] encrypted = cipher.doFinal(data);
        byte[][] ret = new byte[2][];
        ret[0] = encrypted;
        ret[1] = iv;
        return ret;
    }

    public byte[] decrypt(byte[] data, byte[] iv) 
        throws GeneralSecurityException 
    {
        SecretKeySpec skeySpec = new SecretKeySpec(this.raw, "AES");
        IvParameterSpec ivspec = new IvParameterSpec(iv);

        Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
        cipher.init(Cipher.DECRYPT_MODE, skeySpec, ivspec);
        return cipher.doFinal(data);
    }

    public static byte[] genRandom(int numBytes)
    {
        SecureRandom sr = new SecureRandom();
        byte[] b = new byte[numBytes];
        sr.nextBytes(b);
        return b;
    }

    public static byte[] genRandom(byte[] seed) 
        throws GeneralSecurityException 
    {
        KeyGenerator kgen = KeyGenerator.getInstance("AES");
        SecureRandom sr = SecureRandom.getInstance("SHA1PRNG");
        sr.setSeed(seed);
        kgen.init(128, sr); // 192 and 256 bits may not be available
        SecretKey skey = kgen.generateKey();
        byte[] raw = skey.getEncoded();
        return raw;
    }
}
