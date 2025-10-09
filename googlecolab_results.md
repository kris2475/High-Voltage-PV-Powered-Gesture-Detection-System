# Google Colab Experiments: PV-Powered Gesture Recognition

## Overview
These experiments were conducted in Google Colab to evaluate the feasibility of machine learning (ML) models for gesture recognition using photovoltaic (PV) panel shadow signals.  
The workflow included **data loading, preprocessing, visualization, feature scaling, model training, and evaluation** across multiple algorithms.

---

## Data Preparation

1. **File Handling**
   - Successfully read and labeled the following files:
     - `G0_09102025.csv` → Gesture **G0**
     - `G1_09102025.csv` → Gesture **G1**
     - `G2_09102025.csv` → Gesture **G2**
     - `G4_09102025.csv` → Gesture **G4**
     - `G10_09102025.csv` → Gesture **G10**

2. **Gesture Labels**
   - **G0**: No gesture  
   - **G1**: Lower hand  
   - **G2**: Lift hand  
   - **G4**: Left hand swipe left  
   - **G10**: Two hands swimming motion  

3. **Dataset Combination**
   - Combined DataFrame shape: **(136, 3003)** before dropping `Timestamp`.  
   - After dropping: **(136, 3002)**.  

4. **Target Encoding**
   - Mapping:  
     ```
     {'G0': 0, 'G1': 1, 'G10': 2, 'G2': 3, 'G4': 4}
     ```

5. **Train/Test Split**
   - X_train: (108, 3000)  
   - X_test: (28, 3000)  
   - y_train: (108,)  
   - y_test: (28,)  

6. **Feature Scaling**
   - StandardScaler applied *after splitting*.  
   - X_train_scaled: (108, 3000)  
   - X_test_scaled: (28, 3000)  

---

## Models Trained & Results

### 1. Logistic Regression
- **Accuracy:** 0.9643  
- **Classification Report:**
  - G0: Precision 0.86, Recall 1.00, F1 0.92  
  - G1: Precision 1.00, Recall 0.80, F1 0.89  
  - G10: Precision 1.00, Recall 1.00, F1 1.00  
  - G2: Precision 1.00, Recall 1.00, F1 1.00  
  - G4: Precision 1.00, Recall 1.00, F1 1.00  

---

### 2. Random Forest
- **Accuracy:** 0.8929  
- **Classification Report:**
  - G0: Precision 0.75, Recall 1.00, F1 0.86  
  - G1: Precision 0.80, Recall 0.80, F1 0.80  
  - G10: Precision 1.00, Recall 1.00, F1 1.00  
  - G2: Precision 1.00, Recall 1.00, F1 1.00  
  - G4: Precision 1.00, Recall 0.60, F1 0.75  

---

### 3. Support Vector Machine (SVM)
- **Accuracy:** 0.9286  
- **Classification Report:**
  - G0: Precision 1.00, Recall 1.00, F1 1.00  
  - G1: Precision 1.00, Recall 0.80, F1 0.89  
  - G10: Precision 0.75, Recall 1.00, F1 0.86  
  - G2: Precision 1.00, Recall 1.00, F1 1.00  
  - G4: Precision 1.00, Recall 0.80, F1 0.89  

---

### 4. Deep Neural Network (DNN)
- **Accuracy:** 0.8929  
- **Classification Report:**
  - G0: Precision 0.86, Recall 1.00, F1 0.92  
  - G1: Precision 1.00, Recall 0.80, F1 0.89  
  - G10: Precision 0.86, Recall 1.00, F1 0.92  
  - G2: Precision 0.86, Recall 1.00, F1 0.92  
  - G4: Precision 1.00, Recall 0.60, F1 0.75  

---

## Results Summary

| Model                  | Accuracy (Test Set) |
|-------------------------|---------------------|
| **Logistic Regression** | **0.9643** |
| Support Vector Machine  | 0.9286 |
| Random Forest           | 0.8929 |
| Deep Neural Network     | 0.8929 |

---

## Insights

- **Linear Separability:** Gesture classes appear largely linearly separable after scaling, explaining Logistic Regression’s superior performance.  
- **Dataset Size:** With only 136 samples, simpler models (Logistic Regression) are less prone to overfitting than complex ones (RF, DNN).  
- **Class Imbalance & Recall:** Some models struggled with **G4**, where recall dropped to 0.60.  
- **Complex Models:** DNN and RF performed no better than simpler baselines without hyperparameter tuning.  

---

## Conclusion

- **Logistic Regression** delivered the **highest accuracy (96.4%)** and is the most practical candidate for **TinyML deployment** on the Arduino Nano 33 BLE Sense.  
- It balances accuracy with computational efficiency, aligning with the project’s **low-power constraints**.  
- Future work:
  - Hyperparameter tuning of RF, SVM, and DNN  
  - Larger dataset collection for more robust generalization  
  - Feature importance analysis  
  - Visualization of decision boundaries for interpretability  

---
