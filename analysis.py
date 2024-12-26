import sys
import os
import numpy as np
from sklearn.metrics import silhouette_score
import kmeans  
import symnmf as sy
import utils
import math

# Function for printing error and exit
def error():
    print("An Error Has Occurred")
    exit()

# function to get the arguments needed and convert them
def convert_args():
    def k_to_int(k):
        try:
            return int(k)
        except:
            error()

    if len(sys.argv) < 3:
        error()
    k = sys.argv[1]
    file_name = sys.argv[2]
    k = k_to_int(k)
    if not os.path.exists(file_name):
        error()
    return k, file_name

# calcuates the silhouette score
def calculate_silhouette_score(points, labels):
    if len(np.unique(labels)) > 1:
        return silhouette_score(points, labels)
    else:
        return -1  # Silhouette score is not defined for a single cluster

# calls for the kmeans function
def do_kmeans(k, n, d, iter, inputText):
    labels = kmeans.kmeans(k, n, d, iter, inputText)
    return labels

# calls for the symnmf function
def do_symnmf(n, d, points, k):
    # Calculate W
    W_flat = sy.norm(points)    
    W = np.reshape(W_flat, (n, n))
    H = utils.init_H(W,k,n).tolist()
    W = W.tolist()
    try:
        return np.reshape(sy.calcSymnmf(n, k, W, H), (n,k)).tolist()
    except:
        error()
    
# calculates the euclidean distance between two points
def euclidean_distance(point1, point2):
    if len(point1) != len(point2):
        error()
    squared_diffs = [(p1 - p2) ** 2 for p1, p2 in zip(point1, point2)]
    distance = math.sqrt(sum(squared_diffs))
    return distance

# function to get the right centroid index for every point
def get_points_centroid_index(points,centroids):
    res=[]
    for p in points:
        closest_centroid=0
        closest_dist=float('inf')
        for i in range(len(centroids)):
            dist=euclidean_distance(centroids[i],p)
            if dist < closest_dist:
                closest_centroid=i
            closest_dist=min(dist,closest_dist)
        res.append(closest_centroid)
    return res


# calculates and returns the max indices of a list of lists
def index_of_max_in_lists(list_of_lists):
    max_indices = []
    for lst in list_of_lists:
        if len(lst) == 0:
            max_indices.append(None)  # Append None for empty lists
        else:
            max_index = lst.index(max(lst))
            max_indices.append(max_index)
    return max_indices

def main():
    k, file_name = convert_args()
    points= utils.get_points(file_name)
    n=len(points)
    d=len(points[0])

    kmeans_centroids = do_kmeans(k,n,d,300,file_name)
    kmeans_centroids.pop(0)
    kmeans_res=get_points_centroid_index(points,kmeans_centroids)

    symnmf_H = do_symnmf(n,d, points, k)
    symnmf_res=index_of_max_in_lists(symnmf_H)


    print("nmf: %.4f" % silhouette_score(points, symnmf_res))
    print("kmeans: %.4f" % silhouette_score(points, kmeans_res))


if __name__ == "__main__":
    main()
