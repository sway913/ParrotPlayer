package com.lijian.app.player.bean;

/**
 * @Description:
 * @Author: lijian03@bilibili.com
 * @Time: 2019/1/1
 */
public class PaTimeInfo {
    private int currTime;
    private int totalTime;

    public int getCurrTime() {
        return currTime;
    }

    public void setCurrTime(int currTime) {
        this.currTime = currTime;
    }

    public int getTotalTime() {
        return totalTime;
    }

    public void setTotalTime(int totalTime) {
        this.totalTime = totalTime;
    }

    @Override
    public String toString() {
        return "PaTimeInfo totalTime:"+totalTime+" currTime:"+currTime;
    }
}
