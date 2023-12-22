// AUTOGENERATED FILE - DO NOT MODIFY!
// This file was generated by Djinni from derivings.djinni

package com.dropbox.djinni.test;

import java.util.Date;
import javax.annotation.CheckForNull;
import javax.annotation.Nonnull;

public class RecordWithDerivings implements Comparable<RecordWithDerivings> {


    /*package*/ byte mEight;

    /*package*/ short mSixteen;

    /*package*/ int mThirtytwo;

    /*package*/ long mSixtyfour;

    /*package*/ float mFthirtytwo;

    /*package*/ double mFsixtyfour;

    /*package*/ Date mD;

    /*package*/ String mS;

    public RecordWithDerivings(
            byte eight,
            short sixteen,
            int thirtytwo,
            long sixtyfour,
            float fthirtytwo,
            double fsixtyfour,
            @Nonnull Date d,
            @Nonnull String s) {
        this.mEight = eight;
        this.mSixteen = sixteen;
        this.mThirtytwo = thirtytwo;
        this.mSixtyfour = sixtyfour;
        this.mFthirtytwo = fthirtytwo;
        this.mFsixtyfour = fsixtyfour;
        this.mD = d;
        this.mS = s;
    }

    public byte getEight() {
        return mEight;
    }

    public void setEight(byte eight) {
        this.mEight = eight;
    }

    public short getSixteen() {
        return mSixteen;
    }

    public void setSixteen(short sixteen) {
        this.mSixteen = sixteen;
    }

    public int getThirtytwo() {
        return mThirtytwo;
    }

    public void setThirtytwo(int thirtytwo) {
        this.mThirtytwo = thirtytwo;
    }

    public long getSixtyfour() {
        return mSixtyfour;
    }

    public void setSixtyfour(long sixtyfour) {
        this.mSixtyfour = sixtyfour;
    }

    public float getFthirtytwo() {
        return mFthirtytwo;
    }

    public void setFthirtytwo(float fthirtytwo) {
        this.mFthirtytwo = fthirtytwo;
    }

    public double getFsixtyfour() {
        return mFsixtyfour;
    }

    public void setFsixtyfour(double fsixtyfour) {
        this.mFsixtyfour = fsixtyfour;
    }

    @Nonnull
    public Date getD() {
        return mD;
    }

    public void setD(Date d) {
        this.mD = d;
    }

    @Nonnull
    public String getS() {
        return mS;
    }

    public void setS(String s) {
        this.mS = s;
    }

    @Override
    public boolean equals(@CheckForNull Object obj) {
        if (!(obj instanceof RecordWithDerivings)) {
            return false;
        }
        RecordWithDerivings other = (RecordWithDerivings) obj;
        return this.mEight == other.mEight &&
                this.mSixteen == other.mSixteen &&
                this.mThirtytwo == other.mThirtytwo &&
                this.mSixtyfour == other.mSixtyfour &&
                this.mFthirtytwo == other.mFthirtytwo &&
                this.mFsixtyfour == other.mFsixtyfour &&
                this.mD.equals(other.mD) &&
                this.mS.equals(other.mS);
    }

    @Override
    public int hashCode() {
        // Pick an arbitrary non-zero starting value
        int hashCode = 17;
        hashCode = hashCode * 31 + mEight;
        hashCode = hashCode * 31 + mSixteen;
        hashCode = hashCode * 31 + mThirtytwo;
        hashCode = hashCode * 31 + ((int) (mSixtyfour ^ (mSixtyfour >>> 32)));
        hashCode = hashCode * 31 + Float.floatToIntBits(mFthirtytwo);
        hashCode = hashCode * 31 + ((int) (Double.doubleToLongBits(mFsixtyfour) ^ (Double.doubleToLongBits(mFsixtyfour) >>> 32)));
        hashCode = hashCode * 31 + mD.hashCode();
        hashCode = hashCode * 31 + mS.hashCode();
        return hashCode;
    }

    @Override
    public String toString() {
        return "RecordWithDerivings{" +
                "mEight=" + mEight +
                "," + "mSixteen=" + mSixteen +
                "," + "mThirtytwo=" + mThirtytwo +
                "," + "mSixtyfour=" + mSixtyfour +
                "," + "mFthirtytwo=" + mFthirtytwo +
                "," + "mFsixtyfour=" + mFsixtyfour +
                "," + "mD=" + mD +
                "," + "mS=" + mS +
        "}";
    }


    @Override
    public int compareTo(@Nonnull RecordWithDerivings other)  {
        int tempResult;
        if (this.mEight < other.mEight) {
            tempResult = -1;
        } else if (this.mEight > other.mEight) {
            tempResult = 1;
        } else {
            tempResult = 0;
        }
        if (tempResult != 0) {
            return tempResult;
        }
        if (this.mSixteen < other.mSixteen) {
            tempResult = -1;
        } else if (this.mSixteen > other.mSixteen) {
            tempResult = 1;
        } else {
            tempResult = 0;
        }
        if (tempResult != 0) {
            return tempResult;
        }
        if (this.mThirtytwo < other.mThirtytwo) {
            tempResult = -1;
        } else if (this.mThirtytwo > other.mThirtytwo) {
            tempResult = 1;
        } else {
            tempResult = 0;
        }
        if (tempResult != 0) {
            return tempResult;
        }
        if (this.mSixtyfour < other.mSixtyfour) {
            tempResult = -1;
        } else if (this.mSixtyfour > other.mSixtyfour) {
            tempResult = 1;
        } else {
            tempResult = 0;
        }
        if (tempResult != 0) {
            return tempResult;
        }
        if (this.mFthirtytwo < other.mFthirtytwo) {
            tempResult = -1;
        } else if (this.mFthirtytwo > other.mFthirtytwo) {
            tempResult = 1;
        } else {
            tempResult = 0;
        }
        if (tempResult != 0) {
            return tempResult;
        }
        if (this.mFsixtyfour < other.mFsixtyfour) {
            tempResult = -1;
        } else if (this.mFsixtyfour > other.mFsixtyfour) {
            tempResult = 1;
        } else {
            tempResult = 0;
        }
        if (tempResult != 0) {
            return tempResult;
        }
        tempResult = this.mD.compareTo(other.mD);
        if (tempResult != 0) {
            return tempResult;
        }
        tempResult = this.mS.compareTo(other.mS);
        if (tempResult != 0) {
            return tempResult;
        }
        return 0;
    }
}
