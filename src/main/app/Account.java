package app;

import j.io.*;
import j.str.*;

import java.util.*;
import java.security.*;

import org.json.*;

import util.*;

/**
 * An immutable class representing an account.
 */
public class Account
{
    private static final Crypto crypto;

    static
    {
        try
        {
            crypto = new Crypto();
        }
        catch (Exception e)
        {
            System.out.println("Error initializing Crypto. Have you set up an SSH RSA key?");
            throw new RuntimeException(e);
        }
    }

    private final String type;
    private final String user;
    private final String pass;
    private final JSONObject json;

    public Account(JSONObject obj)
        throws JSONException, java.io.UnsupportedEncodingException, 
        GeneralSecurityException
    {
        this.type = obj.getString("type");
        this.user = obj.getString("user");
    
        byte data[] = StrUtil.fromHex(obj.getString("pass"));
        byte iv[] = StrUtil.fromHex(obj.getString("pass-iv"));    
        this.pass = new String( crypto.decrypt(data, iv), "utf-8");

        this.json = new JSONObject();
        this.json.put("type", this.type);
        this.json.put("pass", obj.getString("pass"));
        this.json.put("pass-iv", obj.getString("pass-iv"));
        this.json.put("user", this.user);
           
    }

    public Account(String type, String user, String pass)
        throws JSONException, java.io.UnsupportedEncodingException, 
        GeneralSecurityException
    {
        this.type = type;
        this.user = user;
        this.pass = pass;

        JSONObject obj = new JSONObject();
        obj.put("type", this.type);
        obj.put("user", this.user);

        byte[][] ret = crypto.encrypt(this.pass.getBytes("utf-8"));
        byte[]data = ret[0];
        byte[]iv = ret[1];

        obj.put("pass", StrUtil.toHex(data));
        obj.put("pass-iv", StrUtil.toHex(iv));
        this.json = obj;
    }    

    public String getType()
    {
        return this.type;
    }

    public String getUser()
    {
        return this.user;
    }

    public String getPass()
    {
        return this.pass;
    }

    public JSONObject toJSON()
    {
        return this.json;
    }
}
