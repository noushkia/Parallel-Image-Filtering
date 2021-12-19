# Parallel-Image-Filtering
The third project of OS course offered at the University of Tehran

# Serial Runtime
Time taken for reading image: 2.62 ms<br>
Time taken for blur filter: 27.16 ms<br>
Time taken for sepia filter: 6.38 ms<br>
Time taken for mean filter: 6.91 ms<br>
Time taken for X filter: 0.37 ms<br>
Total time taken for applying filters and writing images: 43.06 ms<br>


# Parallel Runtime
Time taken for reading image: 0.90 ms<br> (Speedup = $\frac{2.62}{0.90} \approx 2.90$)<br>

<table>
  <tr>
    <th>Thread Count</th>
    <th>Time (serial/parallel)</th>
  </tr>
  <tr>
    <td>40</td>
    <td>1.5</td>
  </tr>
  <tr>
    <td>20*</td>
    <td>1.2</td>
  </tr>
  <tr>
    <td>16</td>
    <td>1.2</td>
  </tr>
  <tr>
    <td>10</td>
    <td>1.3</td>
  </tr>
  <tr>
    <td>8</td>
    <td>1.5</td>
  </tr>
  <tr>
    <td>4</td>
    <td>1.6</td>
  </tr>
</table>

<h3>For the filters:</h3>

Time taken for  Time taken for blur filter : 8.02 ms<br> (Speedup = $\frac{27.16}{8.02} \approx 3.38$)<br>

Time taken to write on real_pixel: 3.16 ms -> new hotspot<br>

Time taken for sepia filter: 2.36 ms<br> (Speedup = $\frac{6.38}{2.36} \approx 2.70$)<br>

Time taken to write on real_pixel: 3.12 ms<br>

Time taken for mean filter: 5.05 ms<br> (Speedup = $\frac{6.91}{5.05} \approx 1.36$)<br>

Time taken to write on real_pixel: 3.31 ms<br>

Time taken for X filter: 0.32 ms<br>

<h3>Total Time:</h3>

Total time taken for applying filters and writing images: 38.42 ms<br> (Speedup = $\frac{45.92}{38.42} \approx 1.19$)<br>
