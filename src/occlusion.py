import plotly.express as px
import plotly.graph_objs as go
import open3d as o3d
import numpy as np

def log_or_0(x):
    ret = x.copy()
    ret[ret>0] = np.log(ret[ret>0])
    ret[ret<=0] = 0
    return ret

def _build_edge_array_to_plot(edges_start, edges_end):
    edge_x = []
    edge_y = []
    edge_z = []
    for start, end in zip(edges_start, edges_end):
        x0, y0, z0 = start
        x1, y1, z1 = end
        edge_x.append(x0)
        edge_x.append(x1)
        edge_x.append(None)
        edge_y.append(y0)
        edge_y.append(y1)
        edge_y.append(None)
        edge_z.append(z0)
        edge_z.append(z1)
        edge_z.append(None)
    return edge_x, edge_y, edge_z

def plot_3dlines(edges_start, edges_end, **kwargs):
    edge_x, edge_y, edge_z = _build_edge_array_to_plot(edges_start, edges_end)

    return go.Scatter3d(
        x=edge_x, y=edge_y, z=edge_z,
        **kwargs)

def normalized(a, axis=-1, order=2):
    l2 = np.atleast_1d(np.linalg.norm(a, order, axis))
    l2[l2==0] = 1
    return a / np.expand_dims(l2, axis)


# u = p1 - p0
# v = p2 - p1
# face_center = (p0 + p1 + p2) / 3
# face_normal = np.cross(u, v)
# surface_area = np.linalg.norm(face_normal, axis=-1)/2
# face_normal = face_normal / (2*surface_area)[:, None]

def get_incident_points_per_triangle(vertices, triangles, pointcloud, max_dist_positive=0.001, max_dist_negative=0.001):

    # Triangle points 
    p0 = vertices[triangles][:, 0, :]
    p1 = vertices[triangles][:, 1, :]
    p2 = vertices[triangles][:, 2, :]

    # Sequential (CW) edges 
    u = p1 - p0
    v = p2 - p1
    
    # Calculate normal which also gives surface area
    face_normal = np.cross(u, v)
    surface_area = np.linalg.norm(face_normal, axis=-1)/2
    face_normal = face_normal / (2*surface_area)[:, None]

    # List of transformations to the "triangle coordinate system"
    transform_to_tri_cc = np.stack([np.eye(4) for _ in triangles])

    # For loop because doing this vectorized would explode my PC
    for i in range(len(transform_to_tri_cc)):
        rotate = np.eye(4)
        translate = np.eye(4)

        # Triangle coordinate system with p0 at origin
        # This transform gives WCS (world coord. system) <- TCS (Triangle coordinate system)
        rotate[:3,0] = p1[i]-p0[i]
        rotate[:3,1] = p2[i]-p0[i]
        rotate[:3,2] = face_normal[i]
        rotate[:3,3] = p0[i]

        # We want the other way around so reverse the transform
        transform_to_tri_cc[i] = np.linalg.inv(rotate@translate)


    # Point cloud homogeneous coordinates
    points_h = np.hstack((pointcloud, np.ones((len(pointcloud), 1), )))

    projected_points_per_triangle = []
    per_triangle_resolution = []
    for i, transform in enumerate(transform_to_tri_cc):
        # i is the triangle index
        # project point cloud to i-th triangle
        projected = (transform @ points_h.T).T
        # calculate points lying inside the triangle, with distance along the normal axis d
        # obeying `max_dist_negative < d < max_dist_positive`
        is_within_triangle = projected[
            (projected[:, 0] >= 0)   # positive X
            & (projected[:, 1] >= 0)   # positive Y
            & ((projected[:, 1] + projected[:, 0]) <= 1) # inside diagonal line
            & (
                # Inside normal-distance bound
                ((projected[:, 2] > 0) & (projected[:, 2] <= max_dist_positive))
                | ((projected[:, 2] < 0) & (projected[:, 2] >= max_dist_negative))
            )
        ]
        if len(is_within_triangle) > 0:
            projected_points_per_triangle.append(is_within_triangle)
            per_triangle_resolution.append(len(is_within_triangle))
        else:
            projected_points_per_triangle.append([])
            per_triangle_resolution.append(0)

    per_triangle_resolution = np.stack(per_triangle_resolution)
    return per_triangle_resolution, projected_points_per_triangle
