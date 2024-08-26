package com.example.myapplication;

import android.content.Context;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.Iterator;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.android.LogcatAppender;
import ch.qos.logback.classic.encoder.PatternLayoutEncoder;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.classic.util.ContextInitializer;
import ch.qos.logback.core.Appender;
import ch.qos.logback.core.FileAppender;
import ch.qos.logback.core.joran.spi.JoranException;
import org.slf4j.LoggerFactory;

import ch.qos.logback.classic.Level;

import ch.qos.logback.classic.LoggerContext;
import ch.qos.logback.classic.encoder.PatternLayoutEncoder;
import ch.qos.logback.classic.spi.ILoggingEvent;
import ch.qos.logback.core.FileAppender;
public class LogSaver  {


    Logger logger;
    public static final String LOG_DIRECTORY = "";

    private static Logger createLoggerFor(String string, String file) {

        LoggerContext lc = (LoggerContext) LoggerFactory.getILoggerFactory();
        PatternLayoutEncoder ple = new PatternLayoutEncoder();
        ple.setPattern("%d{HH:mm:ss.SSS} [%thread] %-5level %logger{36} - %msg%n");

        ple.setContext(lc);
        ple.start();

        FileAppender<ILoggingEvent> fileAppender = new FileAppender<>();
        fileAppender.setFile(file);
        fileAppender.setEncoder(ple);
        fileAppender.setContext(lc);
        fileAppender.start();

        LogcatAppender logcatAppender = new LogcatAppender();
        logcatAppender.setEncoder(ple);
        logcatAppender.setContext(lc);
        logcatAppender.start();

        ch.qos.logback.classic.Logger log = (ch.qos.logback.classic.Logger) LoggerFactory.getLogger("test!");
        log.addAppender(fileAppender);
        log.addAppender(logcatAppender);
        log.setLevel(Level.DEBUG);//TRACE - DEBUG - INFO -WARN - ERROR
        log.setAdditive(false);

        return log;
    }

    public LogSaver()
    {
        init();
    }
    private void init()
    {
        try{
            SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yy-MM-dd");
            Date nowDate = new Date(System.currentTimeMillis());
            String strNowDate = simpleDateFormat.format(nowDate);
            logger = createLoggerFor("LOG", "log_"+strNowDate+".log");
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public void reset()
    {
        LoggerContext lc = (LoggerContext) LoggerFactory.getILoggerFactory();
        ContextInitializer ci = new ContextInitializer(lc);
        lc.reset();
        try {
            ci.autoConfig();
        } catch (JoranException e) {
            e.printStackTrace();
        }
    }
}