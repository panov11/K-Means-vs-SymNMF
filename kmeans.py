import sys
import math
# function to calculate the distance between two vectors
def euclidean_distance(vector1_array, vector2_array):
    sum = 0
    for i in range(0, len(vector1_array)):
        inter_sum = vector1_array[i] - vector2_array[i]
        sum += math.pow(vector1_array[i] - vector2_array[i], 2)
    return math.sqrt(sum)

# converting the input to array of float points
def convert_strings_to_floats(input_array):
    input_array[(len(input_array) - 1)] = input_array[(len(input_array) - 1)].rstrip()
    output_array = []
    for element in input_array:
        converted_float = float(element)
        output_array.append(converted_float)
    return output_array


# does the kmeans algorithm
def kmeans(K,N,d,iter,file_name):
    epsilon = 0.001
    input_file = open(file_name,"r")
    data_points_arr = [[] for y in range(N + 1)]
    clusters_arr = [[] for y in range(K + 1)]
    centroids_arr = [[] for y in range(K + 1)]
    iter_count = 0
    is_finished = False
    for i in range(1, N + 1):
        current_line = convert_strings_to_floats(input_file.readline().split(","))
        data_points_arr[i] = current_line
        if i <= K:
            centroids_arr[i] = current_line
    while(iter_count < iter and not is_finished):
        is_finished = True
        clusters_arr = [[] for y in range(K + 1)]
        iter_count += 1
        for i in range(1,N + 1):
            min_distance = math.inf
            min_distance_index = 1
            for j in range(1, K + 1):
                distance = euclidean_distance(data_points_arr[i], centroids_arr[j])
                if distance < min_distance:
                    min_distance = distance
                    min_distance_index = j
            clusters_arr[min_distance_index].append(data_points_arr[i])
        for i in range(1,K+1):
            cluster_size = len(clusters_arr[i])
            new_centroid = [0] * d
            for j in range (0,d):
                sum_d = 0
                for t in range(0,len(clusters_arr[i])):
                    sum_d += clusters_arr[i][t][j]
                new_centroid[j] = (sum_d / cluster_size)
            if euclidean_distance(new_centroid, centroids_arr[i]) >= epsilon:
                is_finished = False
            centroids_arr[i] = new_centroid
    input_file.close()
    return centroids_arr