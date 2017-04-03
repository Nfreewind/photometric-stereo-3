# Computer Vision Hw1 Report

> this file use markdown with Tex, use [markdown-preview-enhanced](https://atom.io/packages/markdown-preview-enhanced) for the correct view

## 程式流程

以下以程式被執行之順序進行說明

### `main` (L36-60)

參數解析，並確認有輸入必要參數等

### `main` (L62-93)

讀取資料，包含`LightSource.txt`與對應的圖片，並儲存成`vector<pair<Vec3f, Mat>>`配對，準備用於計算normal與albedo

### `calcualte_normal` (L152-159)

基於公式

$$
\begin{bmatrix}
    {I_1}{(x,y)} \\
    {I_2}{(x,y)} \\
    \vdots \\
    {I_n}{(x,y)}
\end{bmatrix} =
\begin{bmatrix}
    {S_1}^T \\
    {S_2}^T \\
    \vdots \\
    {S_n}^T
\end{bmatrix}
b{(x,y)}
$$

將光源資料先建成矩陣`S`，並考量到對於不同點其光源資料都是共通，為了避免重複計算，計算`S`之反矩陣`S_sol`($S^{-1}$)，以方便使用以下方式完成計算

$$
b{(x,y)}
=
S^{-1}
\begin{bmatrix}
    {I_1}{(x,y)} \\
    {I_2}{(x,y)} \\
    \vdots \\
    {I_n}{(x,y)}
\end{bmatrix}
$$

### `calcualte_normal` (L177-187)

使用上述段落取得 $b{(x,y)}$，並計算以下參數

$$
\begin{align}
N{(x,y)} = \frac{b{(x,y)}}{\Vert{b{(x,y)}}\Vert} &&
\rho{(x,y)} = \Vert{b{(x,y)}}\Vert
\end{align}
$$

至此，取得albedo與normal map

### `calculate_depth` (L105-115)

使用

$$
\begin{align}
\frac{\partial{f}}{\partial{x}} = \frac{    
    -{N_a}{(x,y)}
}{    {N_c}{(x,y)}    } &&
\frac{\partial{f}}{\partial{y}} = \frac{    
    -{N_b}{(x,y)}
}{    {N_c}{(x,y)}    }
\end{align}
$$

方式計算 `grd_x` ($\frac{\partial{f}}{\partial{x}}$) 與 `grd_y` ($\frac{\partial{f}}{\partial{y}}$)

### `calculate_depth` (L130-134)

計算深度，此部分是以正中心為0往外積分方式進行，並且有進行先列後欄與先欄後列2種狀況，而課程中建議可以用不同方向取平均，而此實作中由於輸出前會進行正規化，為了減少重工，這裡只有相加

由於從中間積分會需要寫4個不同方向的迴圈，本實作將此工作模組化，由`integral`完成

#### `integral` (L96)

如上述所說先列後欄與先欄後列2種狀況，此函式再導向`integral_rtc`與`integral_ctr`

##### `integral_rtc` (L37-60)

先列後欄的積分實作，即是

$$
\begin{align}
\tilde{z} =
\int_{center}^{u}{
    \frac{\partial{f}}{\partial{y}}{(center,y)dy}
} +
\int_{u}^{v}{
    \frac{\partial{f}}{\partial{x}}{(x,y)dx}
}
\end{align}
$$

##### `integral_ctr` (L62-85)

先欄後列的積分實作，即是

$$
\begin{align}
\tilde{z} =
\int_{center}^{v}{
    \frac{\partial{f}}{\partial{x}}{(x,center)dx}
} +
\int_{v}^{u}{
    \frac{\partial{f}}{\partial{y}}{(x,y)dy}
}
\end{align}
$$

### `main` (L107-114)

偷看albedo，沒資料的地方也把depth map歸零

$$
depth{\big[{albedo=0}\big]}=0
$$

### `to_ply`

存成ply檔，結束

## 執行方式

> 參考 Readme

## 自己多做的功能

1. 積分的時候有考慮不同方向的積分
2. 參考albedo修正depth map
