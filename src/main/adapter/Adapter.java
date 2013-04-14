package adapter;

import j.io.*;

import java.io.*;
import java.net.*;
import java.util.*;

import app.*;

public abstract class Adapter
{
    protected final Account acct;

    public Adapter(Account acct)
    {
        if (acct == null)
            throw new IllegalArgumentException("acct is null");

        this.acct = acct;
    }

    public static Adapter create(Account acct)
    {
        String nameLower = acct.getType().toLowerCase();

        if (nameLower.equals("uva"))
        {
            return new UVA(acct);
        }
        
        return null;
    }

    public abstract void login() 
        throws IOException;

    protected abstract void send(String probId, Lang lang, File contents)
        throws IOException;

    public void send(String probId, File file)
        throws IOException
    {
        //
        // auto detect lang
        //
        Lang lang = null;
        String fileName = file.getName();
        int index  = fileName.lastIndexOf('.');
        if (index >=0)
        {
            String ext = fileName.substring(index+1).toLowerCase();
            
            for(Lang l : Lang.values())
            {
                for (String s : l.exts())
                {
                    if (s.equalsIgnoreCase(ext))
                    {
                        lang = l;
                        break;
                    }
                }
            }
        }

        send(probId, lang, file);
    }
}
